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

bool IntersectionPoint(const Vector3& origin, const Vector3& direction, const Vector3& plane, Vector3& intersection)
{
	// Vérifie si la ligne est parallèle au plan
	double dotProduct = direction.Dot(plane);
	if (std::abs(dotProduct) < 1e-6) {
		// La ligne est parallèle au plan
		//std::cerr << "La ligne est parallèle au plan, aucune intersection." << std::endl;
		return false; // ou une autre valeur d'erreur
	}

	// Calcul de la distance le long de la ligne à partir de son origine jusqu'au point d'intersection
	double t = (-origin.Dot(plane)) / dotProduct;

	// Calcul du point d'intersection
	intersection = origin + (direction * t);
	return true;
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

		float* upDownRef = &fwd;
		if (mode2D)
			upDownRef = &upWorld;
		if (InputSystem::GetKey(UP) || InputSystem::GetKey(Z))
			*upDownRef = -1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(DOWN) || InputSystem::GetKey(S))
			*upDownRef = 1 * Time::GetDeltaTime();

		if (!mode2D)
		{
			if (InputSystem::GetKey(A))
				upWorld = 1 * Time::GetDeltaTime();
			else if (InputSystem::GetKey(E))
				upWorld = -1 * Time::GetDeltaTime();
		}

		if (InputSystem::GetKey(RIGHT) || InputSystem::GetKey(D))
			side = 1 * Time::GetDeltaTime();
		else if (InputSystem::GetKey(LEFT) || InputSystem::GetKey(Q))
			side = -1 * Time::GetDeltaTime();

		/*if (mode2D && ImGui::IsMouseDown(ImGuiMouseButton_Right) && startRotatingCamera)
		{
			up += InputSystem::InputSystem::mouseSpeed.y * 1.5f;
			side -= InputSystem::InputSystem::mouseSpeed.x * 1.5f;
		}*/

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

void SceneMenu::GetMouseRay(Vector3& mouseWorldDir, Vector3& mouseWorldDirNormalized, Vector3& worldCoords, std::shared_ptr<Camera>& camera)
{
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();

	// Calculate camera matrix without translate
	Vector3 cameraRotation = cameraTransform->GetRotation();
	glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));

	// Get screen mouse position (inverted)
	glm::vec3 mousePositionGLM = glm::vec3(startAvailableSize.x - mousePosition.x, startAvailableSize.y - (windowSize.y - mousePosition.y), 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	glm::vec3 vec3worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, camera->projection, glm::vec4(0, 0, startAvailableSize.x, startAvailableSize.y));
	worldCoords = Vector3(vec3worldCoords.x, vec3worldCoords.y, vec3worldCoords.z);

	// Normalise direction if needed
	mouseWorldDir = worldCoords;
	mouseWorldDirNormalized = mouseWorldDir.Normalized();
}

Side SceneMenu::GetNearSide(float camDistance,
	const Vector3& rightClosestPointCam, const Vector3& rightClosestPoint,
	const Vector3& upClosestPointCam, const Vector3& upClosestPoint,
	const Vector3& forwardClosestPointCam, const Vector3& forwardClosestPoint)
{
	float rightPointsDist = Vector3::Distance(rightClosestPointCam, rightClosestPoint);
	float upPointsDist = Vector3::Distance(upClosestPointCam, upClosestPoint);
	float forwardPointsDist = Vector3::Distance(forwardClosestPointCam, forwardClosestPoint);
	if (isnan(rightPointsDist))
		rightPointsDist = 99999999.0f;
	if (isnan(upPointsDist))
		upPointsDist = 99999999.0f;
	if (isnan(forwardPointsDist))
		forwardPointsDist = 99999999.0f;

	float distanceDiviser = camDistance;
	if (mode2D)
		distanceDiviser = weakCamera.lock()->GetProjectionSize();

	// Detect the arrow
	Side nearSide = Side_None;
	if (rightPointsDist < upPointsDist && rightPointsDist < forwardPointsDist)
	{
		if (rightPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side_Right;
	}
	else if (upPointsDist < rightPointsDist && upPointsDist < forwardPointsDist)
	{
		if (upPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side_Up;
	}
	else if (forwardPointsDist < upPointsDist && forwardPointsDist < rightPointsDist)
	{
		if (forwardPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side_Forward;
	}
	return nearSide;
}

Side SceneMenu::DetectSide(float camDistance, const Vector3& objectPosition, const Vector3& camPosition, const Vector3& mouseWorldDirNormalized,
	const Vector3& objectRight, const Vector3& objectUp, const Vector3& objectForward)
{
	// Get nearest lines interaction points on the object's direction axis
	Vector3 rightClosestPoint = GetNearestPoint(objectPosition, objectRight, camPosition, mouseWorldDirNormalized);
	Vector3 upClosestPoint = GetNearestPoint(objectPosition, objectUp, camPosition, mouseWorldDirNormalized);
	Vector3 forwardClosestPoint = GetNearestPoint(objectPosition, objectForward, camPosition, mouseWorldDirNormalized);

	// Get nearest lines interaction points on the camera's direction axis
	Vector3 rightClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectRight);
	Vector3 upClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectUp);
	Vector3 forwardClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectForward);

	/*if (cube1.lock())
	{
		cube1.lock()->GetTransform()->SetPosition(rightClosestPoint);
		cube2.lock()->GetTransform()->SetPosition(upClosestPoint);
		cube3.lock()->GetTransform()->SetPosition(forwardClosestPoint);
	}*/

	//----------------------------------- Check if the mouse is on the arrow

	// Check if the side of the arrows is correct

	bool isRightTooFar = abs(objectPosition.x - rightClosestPoint.x) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	bool isUpTooFar = abs(objectPosition.y - upClosestPoint.y) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	bool isForwardTooFar = abs(objectPosition.z - forwardClosestPoint.z) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	if (!mode2D)
	{
		isRightTooFar = abs(objectPosition.x - rightClosestPoint.x) > 1 * camDistance / 8.0f;
		isUpTooFar = abs(objectPosition.y - upClosestPoint.y) > 1 * camDistance / 8.0f;
		isForwardTooFar = abs(objectPosition.z - forwardClosestPoint.z) > 1 * camDistance / 8.0f;
	}

	Side nearSide = GetNearSide(camDistance, rightClosestPointCam, rightClosestPoint, upClosestPointCam, upClosestPoint, forwardClosestPointCam, forwardClosestPoint);

	float rightRightSide = Vector3::Dot((rightClosestPoint - objectPosition).Normalize(), objectRight);
	float upRightSide = Vector3::Dot((upClosestPoint - objectPosition).Normalize(), objectUp);
	float forwardRightSide = Vector3::Dot((forwardClosestPoint - objectPosition).Normalize(), objectForward);

	Side tempSide = Side_None;
	if (rightRightSide >= 0.95f && nearSide == Side_Right && !isRightTooFar)
	{
		tempSide = Side_Right;
	}
	else if (upRightSide >= 0.95f && nearSide == Side_Up && !isUpTooFar)
	{
		tempSide = Side_Up;
	}
	else if (forwardRightSide >= 0.95f && nearSide == Side_Forward && !isForwardTooFar)
	{
		tempSide = Side_Forward;
	}
	return tempSide;
}

void SceneMenu::CheckAllowRotation(float dist, bool& allowRotation, bool isIntersectionGood, Side sideToCheck, const Vector3& intersection)
{
	if (!allowRotation && isIntersectionGood)
	{
		if (dist >= 0.10f && dist <= 0.13f)
		{
			startDragPos = intersection;
			side = sideToCheck;
			allowRotation = true;
		}
		else
		{
			allowRotation = false;
		}
	}
}

void SceneMenu::ProcessTool(std::shared_ptr<Camera>& camera)
{
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();

	//------------------------------------------------------------------------- Detect user inputs to apply tool's behaviour on the selected gameobject
	Vector3 worldCoords;
	Vector3 mouseWorldDir;
	Vector3 mouseWorldDirNormalized;

	GetMouseRay(mouseWorldDir, mouseWorldDirNormalized, worldCoords, camera);
	mouseWorldDir *= -1;
	mouseWorldDirNormalized *= -1;

	// Store world mouse position
	oldWorldMousePosition = worldMousePosition;
	worldMousePosition = mouseWorldDir;

	if (mode2D && toolMode != Tool_MoveCamera)
		mouseWorldDirNormalized = Vector3(0, 0, -1);

	// Move the camera if the mouse left button is held
	if ((InputSystem::GetKey(MOUSE_LEFT) && toolMode == Tool_MoveCamera) || InputSystem::GetKey(MOUSE_MIDDLE) || (ImGui::IsMouseDown(ImGuiMouseButton_Right) && mode2D))
	{
		float tempCameraHandMoveSpeed = cameraHandMoveSpeed;
		if (mode2D)
			tempCameraHandMoveSpeed = 1;

		Vector3 newPos = cameraTransform->GetPosition() + (oldWorldMousePosition - worldMousePosition) * tempCameraHandMoveSpeed;
		cameraTransform->SetPosition(newPos);
	}

	if (toolMode != Tool_MoveCamera)
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
				side = DetectSide(camDistance, objectPosition, camPosition, mouseWorldDirNormalized, objectRight, objectUp, objectForward);
			}

			if (InputSystem::GetKey(MOUSE_LEFT) && (side != Side_None || toolMode == Tool_Rotate))
			{
				Vector3 planeNormalX = Vector3(1, 0, 0);
				Vector3 planeNormalY = Vector3(0, 1, 0);
				Vector3 planeNormalZ = Vector3(0, 0, 1);

				Vector3 intersectionX = Vector3(0);
				Vector3 intersectionY = Vector3(0);
				Vector3 intersectionZ = Vector3(0);
				Vector3 rayStartPosition = cameraTransform->GetPosition() - Editor::GetSelectedGameObject()->GetTransform()->GetPosition();
				if (mode2D)
					rayStartPosition += mouseWorldDir;
				bool isIntersectionXGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalX, intersectionX);
				bool isIntersectionYGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalY, intersectionY);
				bool isIntersectionZGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalZ, intersectionZ);

				Vector3 objectDir = Vector3(0);

				// Select the right value
				if (side == Side_Right)
					objectDir = objectRight;
				else if (side == Side_Up)
					objectDir = objectUp;
				else if (side == Side_Forward)
					objectDir = objectForward;

				// Get the closest point on the object axis
				Vector3 closestPoint = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectDir);

				if (InputSystem::GetKeyDown(MOUSE_LEFT))
				{
					// Get start object value
					if (toolMode == Tool_Move)
						startObjectValue = selectedGoTransform->GetPosition();
					else if (toolMode == Tool_Rotate)
						startObjectValue = selectedGoTransform->GetRotation();
					else if (toolMode == Tool_Scale)
						startObjectValue = selectedGoTransform->GetLocalScale();

					startMovePosition = closestPoint;
					finalAngle = 0;


					allowRotation = false;
					if (toolMode == Tool_Rotate)
					{
						float distanceDiviser = camDistance;
						if (mode2D)
							distanceDiviser = camera->GetProjectionSize() * 1.5f;

						float distX = Vector3::Distance(Vector3(0), intersectionX) / distanceDiviser;
						float distY = Vector3::Distance(Vector3(0), intersectionY) / distanceDiviser;
						float distZ = Vector3::Distance(Vector3(0), intersectionZ) / distanceDiviser;

						CheckAllowRotation(distX, allowRotation, isIntersectionXGood, Side_Right, intersectionX);
						CheckAllowRotation(distY, allowRotation, isIntersectionYGood, Side_Up, intersectionY);
						CheckAllowRotation(distZ, allowRotation, isIntersectionZGood, Side_Forward, intersectionZ);
					}
				}

				//Set object position/rotation/scale
				if (toolMode == Tool_Move)
				{
					// Calculate the value offset
					Vector3 objectOffset = (closestPoint - startMovePosition);

					// Snap values if needed
					if (InputSystem::GetKey(LEFT_CONTROL))
					{
						objectOffset.x = (int)(objectOffset.x / snapAmount) * snapAmount;
						objectOffset.y = (int)(objectOffset.y / snapAmount) * snapAmount;
						objectOffset.z = (int)(objectOffset.z / snapAmount) * snapAmount;
					}
					selectedGoTransform->SetPosition(startObjectValue + objectOffset);
				}
				else if (toolMode == Tool_Rotate && allowRotation)
				{
					Vector3 finalIntersection;
					if (side == Side_Right)
						finalIntersection = intersectionX;
					else if (side == Side_Up)
						finalIntersection = intersectionY;
					else if (side == Side_Forward)
						finalIntersection = intersectionZ;

					if (finalIntersection != startDragPos)
					{
						float angle = startDragPos.Dot(finalIntersection) / (startDragPos.Magnitude() * finalIntersection.Magnitude());
						float angleDeg = acos(angle) * 180.0f / M_PI;
						if (!isnan(angleDeg))
						{
							float crossProduct = 0;

							if (side == Side_Right)
								crossProduct = startDragPos.z * finalIntersection.y - startDragPos.y * finalIntersection.z;
							else if (side == Side_Up)
								crossProduct = startDragPos.x * finalIntersection.z - startDragPos.z * finalIntersection.x;
							else if (side == Side_Forward)
								crossProduct = startDragPos.y * finalIntersection.x - startDragPos.x * finalIntersection.y;

							if (crossProduct < 0)
								finalAngle += angleDeg;
							else
								finalAngle -= angleDeg;

							if (side == Side_Right)
								selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(finalAngle, 0, 0));
							else if (side == Side_Up)
								selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, finalAngle, 0));
							else if (side == Side_Forward)
								selectedGoTransform->SetLocalRotation(startObjectValue + Vector3(0, 0, finalAngle));
						}
						startDragPos = finalIntersection;
					}
				}
				else if (toolMode == Tool_Scale)
				{
					Vector3 objectOffset = (closestPoint - startMovePosition);
					objectOffset *= 4;
					selectedGoTransform->SetLocalScale(startObjectValue + objectOffset);
				}
			}
		}
	}
}

Vector3 SceneMenu::GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2)
{
	Vector3 ClosestPoint;
	if (lineDir2 != Vector3(0))
	{
		Vector3 V = linePos2 - linePos1;

		float dotD1D2 = Vector3::Dot(lineDir1, lineDir2);

		float a = Vector3::Dot(lineDir1, V);
		float b = Vector3::Dot(lineDir2, V);
		float c = Vector3::Dot(lineDir1, lineDir2);

		float s = (a - b * dotD1D2) / (1 - dotD1D2 * dotD1D2);
		float t = (b - c * s);

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
			Editor::isToolLocalMode = true;
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(toolMode == Tool_Scale);
		if (ImGui::Button("Scale"))
		{
			toolMode = Tool_Scale;
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(mode2D);
		if (ImGui::Button("2D"))
		{
			Switch2DMode(!mode2D);
		}
		ImGui::PopStyleColor(3);
		SetButtonColor(false);
		if (Editor::isToolLocalMode)
		{
			if (ImGui::Button("Local"))
			{
				Editor::isToolLocalMode = false;
			}
		}
		else
		{
			if (ImGui::Button("World"))
			{
				Editor::isToolLocalMode = true;
			}
		}
		ImGui::PopStyleColor(3);
	}
}
