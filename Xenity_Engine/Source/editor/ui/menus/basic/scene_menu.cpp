// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "scene_menu.h"

#include <imgui/imgui.h>
#include <glm/gtx/quaternion.hpp>

#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/graphics/graphics.h>
#include <engine/inputs/input_system.h>
#include <engine/graphics/camera.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/rect_transform.h>
#include <engine/time/time.h>
#include <engine/engine_settings.h>
#include <engine/math/math.h>
#include <engine/game_elements/gameplay_manager.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/tools/internal_math.h>
#include <engine/graphics/texture/texture_default.h>
#include <engine/game_elements/prefab.h>
#include <editor/ui/editor_icons.h>


void SceneMenu::Init()
{
	m_cameraGO = CreateGameObjectEditor("Camera");
	std::shared_ptr<Camera> cameraLock = m_cameraGO.lock()->AddComponent<Camera>();
	cameraLock->SetNearClippingPlane(0.01f);
	cameraLock->SetFarClippingPlane(2000);
	cameraLock->SetProjectionSize(5.0f);
	cameraLock->SetFov(70);
	cameraLock->SetIsEditor(true);
	cameraLock->GetTransform()->SetPosition(Vector3(0, 1, 0));
	weakCamera = cameraLock;
}

bool IntersectionPoint(const Vector3& origin, const Vector3& direction, const Vector3& plane, Vector3& intersection)
{
	// V�rifie si la ligne est parall�le au plan
	const float dotProduct = direction.Dot(plane);
	if (std::abs(dotProduct) < 1e-6) {
		// La ligne est parall�le au plan
		//std::cerr << "La ligne est parall�le au plan, aucune intersection." << std::endl;
		return false; // ou une autre valeur d'erreur
	}

	// Calcul de la distance le long de la ligne � partir de son origine jusqu'au point d'intersection
	const float t = static_cast<float>(-origin.Dot(plane)) / dotProduct;

	// Calcul du point d'intersection
	intersection = origin + (direction * t);
	return true;
}

void SceneMenu::MoveCamera()
{
	// Check user input and camera movement when in the scene menu
	if (InputSystem::GetKeyUp(KeyCode::MOUSE_RIGHT) || InputSystem::GetKeyUp(KeyCode::MOUSE_MIDDLE))
	{
		startRotatingCamera = false;
	}

	if (m_isFocused)
	{
		// Get camera transform
		std::shared_ptr<Transform> cameraTransform = m_cameraGO.lock()->GetTransform();
		Vector3 pos = cameraTransform->GetPosition();

		if ((ImGui::IsMouseClicked(ImGuiMouseButton_Right) || ImGui::IsMouseClicked(ImGuiMouseButton_Middle)) && m_isHovered)
		{
			startRotatingCamera = true;
		}

		// Rotate the camera when dragging the mouse right click


		// Move the camera when using keyboard's arrows
		float fwd = 0;
		float up = 0;
		float upWorld = 0;
		float side = 0;

		float* upDownRef = &fwd;

		if (!InputSystem::GetKey(KeyCode::LEFT_CONTROL)) // Disable camera keyboard controls while making a shortcut
		{
			if (m_mode2D)
				upDownRef = &upWorld;
			if (InputSystem::GetKey(KeyCode::UP) || InputSystem::GetKey(KeyCode::Z))
				*upDownRef = -1 * Time::GetDeltaTime();
			else if (InputSystem::GetKey(KeyCode::DOWN) || InputSystem::GetKey(KeyCode::S))
				*upDownRef = 1 * Time::GetDeltaTime();

			if (!m_mode2D)
			{
				if (InputSystem::GetKey(KeyCode::A))
					upWorld = 1 * Time::GetDeltaTime();
				else if (InputSystem::GetKey(KeyCode::E))
					upWorld = -1 * Time::GetDeltaTime();
			}

			if (InputSystem::GetKey(KeyCode::RIGHT) || InputSystem::GetKey(KeyCode::D))
				side = 1 * Time::GetDeltaTime();
			else if (InputSystem::GetKey(KeyCode::LEFT) || InputSystem::GetKey(KeyCode::Q))
				side = -1 * Time::GetDeltaTime();
		}

		// Move the camera when using the mouse's wheel (Do not use delta time)
		if (m_isHovered)
		{
			if (!m_mode2D)
			{
				if (InputSystem::GetKey(KeyCode::LEFT_SHIFT))
				{
					Editor::s_cameraSpeed += InputSystem::mouseWheel;
					if (Editor::s_cameraSpeed < 1.0f)
					{
						Editor::s_cameraSpeed = 1.0f;
					}

					if (Editor::s_cameraSpeed > 100)
					{
						Editor::s_cameraSpeed = 100;
					}
				}
				else
				{
					fwd -= InputSystem::mouseWheel / 15.0f;
				}
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
		pos -= cameraTransform->GetForward() * (fwd / 7.0f) * Editor::s_cameraSpeed;
		pos -= cameraTransform->GetLeft() * (side / 7.0f) * Editor::s_cameraSpeed;
		pos -= cameraTransform->GetUp() * (up / 7.0f) * Editor::s_cameraSpeed;
		pos.y -= (upWorld / 7.0f) * Editor::s_cameraSpeed;

		cameraTransform->SetPosition(pos);

		// Rotate the camera when dragging the mouse right click
		if (!m_mode2D && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			const Quaternion& rotQ = cameraTransform->GetRotation();
			Quaternion rotX = Quaternion::AngleAxis(-InputSystem::mouseSpeed.y * 70, Vector3(1, 0, 0));
			Quaternion rotY = Quaternion::AngleAxis(InputSystem::mouseSpeed.x * 70, Vector3(0, 1, 0));

			Quaternion newRot = rotY * rotQ * rotX;
			cameraTransform->SetRotation(newRot);
		}
		else if (m_mode2D)
		{
			cameraTransform->SetRotation(Quaternion::Identity());
		}
	}
}

bool getHitDistance(const Vector3& corner1, const Vector3& corner2, const Vector3& dirfrac, const Vector3& startPosition, float* t)
{
	float t1 = (corner1.x - startPosition.x) * dirfrac.x;
	float t2 = (corner2.x - startPosition.x) * dirfrac.x;
	float t3 = (corner1.y - startPosition.y) * dirfrac.y;
	float t4 = (corner2.y - startPosition.y) * dirfrac.y;
	float t5 = (corner1.z - startPosition.z) * dirfrac.z;
	float t6 = (corner2.z - startPosition.z) * dirfrac.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us, if tmin > tmax, ray doesn't intersect AABB
	if (tmax < 0 || tmin > tmax)
	{
	}
	else
	{
		if (*t > tmin)
		{
			*t = tmin;
			return true;
		}
	}
	return false;
}

bool HitGameObjectComparator(const SceneMenu::HitGameObjectInfo& c1, const SceneMenu::HitGameObjectInfo& c2)
{
	return c2.distance > c1.distance;
}

std::vector<SceneMenu::HitGameObjectInfo> SceneMenu::CheckBoundingBoxesOnClick(Camera& camera)
{
	const Vector3 dir = camera.GetMouseRay();

	Vector3 dirfrac;
	dirfrac.x = 1.0f / dir.x;
	dirfrac.y = 1.0f / dir.y;
	dirfrac.z = 1.0f / dir.z;

	const Vector3& camPos = camera.GetTransform()->GetPosition();

	float dis = 999999;
	float minDis = dis;
	std::shared_ptr<GameObject> newGameObject = nullptr;
	const int gameObjectCount = GameplayManager::gameObjectCount;
	std::vector<HitGameObjectInfo> selectedGOs;
	for (int i = 0; i < gameObjectCount; i++)
	{
		const std::shared_ptr<GameObject>& selectedGO = GameplayManager::GetGameObjects()[i];
		const std::shared_ptr<MeshRenderer> meshRenderer = selectedGO->GetComponent<MeshRenderer>();

		if (meshRenderer && meshRenderer->GetMeshData() && selectedGO->IsLocalActive() && meshRenderer->IsEnabled())
		{
			const Vector3& min = meshRenderer->GetMeshData()->GetMinBoundingBox();
			const Vector3& max = meshRenderer->GetMeshData()->GetMaxBoundingBox();
			Vector3 transformedMin = Vector3(selectedGO->GetTransform()->GetTransformationMatrix() * glm::vec4(min.x, min.y, min.z, 1));
			Vector3 transformedMax = Vector3(selectedGO->GetTransform()->GetTransformationMatrix() * glm::vec4(max.x, max.y, max.z, 1));
			transformedMin.x = -transformedMin.x;
			transformedMax.x = -transformedMax.x;

			dis = 999999;
			const bool hit = getHitDistance(transformedMin, transformedMax, dirfrac, camPos, &dis);

			if (hit)
			{
				HitGameObjectInfo hitInfo;
				hitInfo.gameObject = selectedGO;

				// If the camera is inside the object, we invert the distance and make the object in the end of the list
				if (dis < 0)
				{
					dis = -dis * 10000;
				}

				hitInfo.distance = dis;
				selectedGOs.push_back(hitInfo);
				if (dis < minDis)
				{
					minDis = dis;
					newGameObject = selectedGO;
				}
			}
		}
	}
	std::sort(selectedGOs.begin(), selectedGOs.end(), HitGameObjectComparator);
	return selectedGOs;
}

void SceneMenu::GetMouseRay(Vector3& mouseWorldDir, Vector3& mouseWorldDirNormalized, Vector3& worldCoords, Camera& camera)
{
	const std::shared_ptr<Transform> cameraTransform = camera.GetTransform();

	const Quaternion& baseQ = cameraTransform->GetRotation();

	glm::mat4 cameraModelMatrix = glm::toMat4(glm::quat(baseQ.w, -baseQ.x, -baseQ.y, -baseQ.z));

	// Get screen mouse position (inverted)
	const glm::vec3 mousePositionGLM = glm::vec3(m_startAvailableSize.x - m_mousePosition.x, m_startAvailableSize.y - (m_windowSize.y - m_mousePosition.y), 0.0f); // Invert Y for OpenGL coordinates

	// Get world mouse position (position at the near clipping plane)
	worldCoords = Vector3(glm::unProject(mousePositionGLM, cameraModelMatrix, camera.GetProjection(), glm::vec4(0, 0, m_startAvailableSize.x, m_startAvailableSize.y)));

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
	if (std::isnan(rightPointsDist))
		rightPointsDist = 99999999.0f;
	if (std::isnan(upPointsDist))
		upPointsDist = 99999999.0f;
	if (std::isnan(forwardPointsDist))
		forwardPointsDist = 99999999.0f;

	float distanceDiviser = camDistance;
	if (m_mode2D)
		distanceDiviser = weakCamera.lock()->GetProjectionSize();

	// Detect the arrow
	Side nearSide = Side::Side_None;
	if (rightPointsDist < upPointsDist && rightPointsDist < forwardPointsDist)
	{
		if (rightPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side::Side_Right;
	}
	else if (upPointsDist < rightPointsDist && upPointsDist < forwardPointsDist)
	{
		if (upPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side::Side_Up;
	}
	else if (forwardPointsDist < upPointsDist && forwardPointsDist < rightPointsDist)
	{
		if (forwardPointsDist / distanceDiviser <= 0.02f)
			nearSide = Side::Side_Forward;
	}
	return nearSide;
}

Side SceneMenu::DetectSide(float camDistance, const Vector3& objectPosition, const Vector3& camPosition, const Vector3& mouseWorldDirNormalized,
	const Vector3& objectRight, const Vector3& objectUp, const Vector3& objectForward)
{
	// Get nearest lines interaction points on the object's direction axis
	const Vector3 rightClosestPoint = GetNearestPoint(objectPosition, objectRight, camPosition, mouseWorldDirNormalized);
	const Vector3 upClosestPoint = GetNearestPoint(objectPosition, objectUp, camPosition, mouseWorldDirNormalized);
	const Vector3 forwardClosestPoint = GetNearestPoint(objectPosition, objectForward, camPosition, mouseWorldDirNormalized);

	// Get nearest lines interaction points on the camera's direction axis
	const Vector3 rightClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectRight);
	const Vector3 upClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectUp);
	const Vector3 forwardClosestPointCam = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectForward);

	//----------------------------------- Check if the mouse is on the arrow

	// Check if the side of the arrows is correct

	bool isRightTooFar = fabs(objectPosition.x - rightClosestPoint.x) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	bool isUpTooFar = fabs(objectPosition.y - upClosestPoint.y) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	bool isForwardTooFar = fabs(objectPosition.z - forwardClosestPoint.z) > 1 * weakCamera.lock()->GetProjectionSize() / 5.0f;
	if (!m_mode2D)
	{
		isRightTooFar = fabs(objectPosition.x - rightClosestPoint.x) > 1 * camDistance / 8.0f;
		isUpTooFar = fabs(objectPosition.y - upClosestPoint.y) > 1 * camDistance / 8.0f;
		isForwardTooFar = fabs(objectPosition.z - forwardClosestPoint.z) > 1 * camDistance / 8.0f;
	}

	const Side nearSide = GetNearSide(camDistance, rightClosestPointCam, rightClosestPoint, upClosestPointCam, upClosestPoint, forwardClosestPointCam, forwardClosestPoint);

	const double rightRightSide = Vector3::Dot((rightClosestPoint - objectPosition).Normalize(), objectRight);
	const double upRightSide = Vector3::Dot((upClosestPoint - objectPosition).Normalize(), objectUp);
	const double forwardRightSide = Vector3::Dot((forwardClosestPoint - objectPosition).Normalize(), objectForward);

	Side tempSide = Side::Side_None;
	if (rightRightSide >= 0.95 && nearSide == Side::Side_Right && !isRightTooFar)
	{
		tempSide = Side::Side_Right;
	}
	else if (upRightSide >= 0.95 && nearSide == Side::Side_Up && !isUpTooFar)
	{
		tempSide = Side::Side_Up;
	}
	else if (forwardRightSide >= 0.95 && nearSide == Side::Side_Forward && !isForwardTooFar)
	{
		tempSide = Side::Side_Forward;
	}
	return tempSide;
}

void SceneMenu::CheckAllowRotation(float dist, bool& allowRotation, bool isIntersectionGood, Side sideToCheck, const Vector3& intersection)
{
	if (!allowRotation && isIntersectionGood)
	{
		if (dist >= 0.10f && dist <= 0.13f)
		{
			m_startDragPos = intersection;
			m_side = sideToCheck;
			allowRotation = true;
		}
		else
		{
			allowRotation = false;
		}
	}
}

void SceneMenu::ProcessTool(std::shared_ptr<Camera>& camera, bool allowDeselection)
{
	std::shared_ptr<Transform> cameraTransform = camera->GetTransform();

	//------------------------------------------------------------------------- Detect user inputs to apply tool's behaviour on the selected gameobject
	Vector3 worldCoords;
	Vector3 mouseWorldDir;
	Vector3 mouseWorldDirNormalized;

	GetMouseRay(mouseWorldDir, mouseWorldDirNormalized, worldCoords, *camera);
	mouseWorldDir *= -1;
	mouseWorldDirNormalized *= -1;

	// Store world mouse position
	m_oldWorldMousePosition = m_worldMousePosition;
	m_worldMousePosition = mouseWorldDir;

	if (m_mode2D && toolMode != ToolMode::Tool_MoveCamera)
		mouseWorldDirNormalized = Vector3(0, 0, -1);

	std::shared_ptr<GameObject> newGameObjectSelected = nullptr;
	if (InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT))
	{
		const std::vector<HitGameObjectInfo> selectedGOs = CheckBoundingBoxesOnClick(*camera);

		bool hasListChanged = false;
		const size_t selectedGoSize = selectedGOs.size();
		if (selectedGoSize != m_lastHitGameObjects.size())
		{
			hasListChanged = true;
		}
		else
		{
			for (size_t i = 0; i < selectedGoSize; i++)
			{
				if (m_lastHitGameObjects.size() <= i || (selectedGOs[i].gameObject.lock() != m_lastHitGameObjects[i].gameObject.lock()))
				{
					hasListChanged = true;
					break;
				}
			}
		}

		if (hasListChanged || m_selectedGameObjectIndex >= selectedGoSize)
		{
			m_selectedGameObjectIndex = 0;
		}

		if (selectedGoSize > 0)
		{
			newGameObjectSelected = selectedGOs[m_selectedGameObjectIndex].gameObject.lock();
			m_selectedGameObjectIndex++;
		}

		m_lastHitGameObjects = selectedGOs;
	}

	// Move the camera if the mouse left button is held
	if ((InputSystem::GetKey(KeyCode::MOUSE_LEFT) && toolMode == ToolMode::Tool_MoveCamera) || InputSystem::GetKey(KeyCode::MOUSE_MIDDLE) || (ImGui::IsMouseDown(ImGuiMouseButton_Right) && m_mode2D))
	{
		float tempCameraHandMoveSpeed = m_cameraHandMoveSpeed;
		if (m_mode2D)
			tempCameraHandMoveSpeed = 1;

		const Vector3 newPos = cameraTransform->GetPosition() + (m_oldWorldMousePosition - m_worldMousePosition) * tempCameraHandMoveSpeed;
		cameraTransform->SetPosition(newPos);
	}

	if (toolMode != ToolMode::Tool_MoveCamera)
	{
		std::shared_ptr<GameObject> selectedGO = nullptr;
		if (Editor::GetSelectedGameObjects().size() == 1)
			selectedGO = Editor::GetSelectedGameObjects()[0].lock();

		if (selectedGO)
		{
			std::shared_ptr<Transform> selectedGoTransform = selectedGO->GetTransform();

			const Vector3& objectPosition = selectedGoTransform->GetPosition();
			//float xOff = (-Graphics::usedCamera.lock()->GetAspectRatio() * 5) + (objectPosition.x * (Graphics::usedCamera.lock()->GetAspectRatio() * 10));
			//float yOff = (-1 * 5) + (objectPosition.y * (1 * 10));
			//objectPosition = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane

			//objectPosition
			Vector3 camPosition = cameraTransform->GetPosition();
			if (m_mode2D)
				camPosition = Vector3(-worldCoords.x, -worldCoords.y, -worldCoords.z) + camPosition;

			Vector3 objectRight = Vector3(1, 0, 0);
			Vector3 objectUp = Vector3(0, 1, 0);
			Vector3 objectForward = Vector3(0, 0, 1);
			if (!Editor::s_isToolLocalMode)
			{
				// Get selected gameObject directions
				objectRight = selectedGoTransform->GetRight();
				objectUp = selectedGoTransform->GetUp();
				objectForward = selectedGoTransform->GetForward();
			}

			const float camDistance = Vector3::Distance(objectPosition, camPosition);
			if (InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT))
			{
				m_side = DetectSide(camDistance, objectPosition, camPosition, mouseWorldDirNormalized, objectRight, objectUp, objectForward);

				m_oldTransformPosition = selectedGoTransform->GetPosition();
				m_oldTransformRotation = selectedGoTransform->GetLocalEulerAngles();
				m_oldTransformScale = selectedGoTransform->GetLocalScale();

				const std::shared_ptr<RectTransform> rect = selectedGO->GetComponent<RectTransform>();
				if (rect)
				{
					m_oldRectTransformPosition = rect->position;
				}
			}

			if (InputSystem::GetKey(KeyCode::MOUSE_LEFT) && (m_side != Side::Side_None || toolMode == ToolMode::Tool_Rotate))
			{
				Vector3 planeNormalX = Vector3(1, 0, 0);
				Vector3 planeNormalY = Vector3(0, 1, 0);
				Vector3 planeNormalZ = Vector3(0, 0, 1);

				if (!Editor::s_isToolLocalMode && toolMode == ToolMode::Tool_Rotate)
				{
					planeNormalX = objectRight;
					planeNormalY = objectUp;
					planeNormalZ = objectForward;
				}

				Vector3 intersectionX = Vector3(0);
				Vector3 intersectionY = Vector3(0);
				Vector3 intersectionZ = Vector3(0);
				Vector3 rayStartPosition = cameraTransform->GetPosition() - selectedGoTransform->GetPosition();
				if (m_mode2D)
					rayStartPosition += mouseWorldDir;

				const bool isIntersectionXGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalX, intersectionX);
				const bool isIntersectionYGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalY, intersectionY);
				const bool isIntersectionZGood = IntersectionPoint(rayStartPosition, mouseWorldDirNormalized, planeNormalZ, intersectionZ);

				Vector3 objectDir = Vector3(0);

				// Select the right value
				if (m_side == Side::Side_Right)
					objectDir = objectRight;
				else if (m_side == Side::Side_Up)
					objectDir = objectUp;
				else if (m_side == Side::Side_Forward)
					objectDir = objectForward;

				// Get the closest point on the object axis
				const Vector3 closestPoint = GetNearestPoint(camPosition, mouseWorldDirNormalized, objectPosition, objectDir);

				if (InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT))
				{
					// Get start object value
					if (toolMode == ToolMode::Tool_Move)
					{
						const std::shared_ptr<RectTransform> rect = selectedGO->GetComponent<RectTransform>();
						if (rect)
						{
							m_startObjectValue = Vector3(rect->position.x, rect->position.y, 0);
						}
						else
							m_startObjectValue = selectedGoTransform->GetPosition();
					}
					else if (toolMode == ToolMode::Tool_Rotate)
						//startObjectValue = selectedGoTransform->GetLocalEulerAngles();
						m_startObjectRotation = selectedGoTransform->GetLocalRotation();
					else if (toolMode == ToolMode::Tool_Scale)
						m_startObjectValue = selectedGoTransform->GetLocalScale();

					m_startMovePosition = closestPoint;
					m_finalAngle = 0;

					m_allowRotation = false;
					if (toolMode == ToolMode::Tool_Rotate)
					{
						float distanceDiviser = camDistance;
						if (m_mode2D)
							distanceDiviser = camera->GetProjectionSize() * 1.5f;

						const float distX = Vector3::Distance(Vector3(0), intersectionX) / distanceDiviser;
						const float distY = Vector3::Distance(Vector3(0), intersectionY) / distanceDiviser;
						const float distZ = Vector3::Distance(Vector3(0), intersectionZ) / distanceDiviser;

						CheckAllowRotation(distX, m_allowRotation, isIntersectionXGood, Side::Side_Right, intersectionX);
						CheckAllowRotation(distY, m_allowRotation, isIntersectionYGood, Side::Side_Up, intersectionY);
						CheckAllowRotation(distZ, m_allowRotation, isIntersectionZGood, Side::Side_Forward, intersectionZ);
					}
				}

				//Set object position/rotation/scale
				if (toolMode == ToolMode::Tool_Move)
				{
					// Calculate the value offset
					Vector3 objectOffset = (closestPoint - m_startMovePosition);

					// Snap values if needed
					if (InputSystem::GetKey(KeyCode::LEFT_CONTROL))
					{
						objectOffset.x = (int)(objectOffset.x / m_snapAmount) * m_snapAmount;
						objectOffset.y = (int)(objectOffset.y / m_snapAmount) * m_snapAmount;
						objectOffset.z = (int)(objectOffset.z / m_snapAmount) * m_snapAmount;
					}
					const std::shared_ptr<RectTransform> rect = selectedGO->GetComponent<RectTransform>();
					if (rect)
					{
						rect->position.x = m_startObjectValue.x + objectOffset.x / (Graphics::usedCamera->GetAspectRatio() * 10.0f);
						rect->position.y = m_startObjectValue.y - objectOffset.y / 10.0f;
					}
					else
					{
						selectedGoTransform->SetPosition(m_startObjectValue + objectOffset);
					}
				}
				else if (toolMode == ToolMode::Tool_Rotate && m_allowRotation)
				{
					Vector3 finalIntersection;
					if (m_side == Side::Side_Right)
						finalIntersection = intersectionX;
					else if (m_side == Side::Side_Up)
						finalIntersection = intersectionY;
					else if (m_side == Side::Side_Forward)
						finalIntersection = intersectionZ;

					if (finalIntersection != m_startDragPos)
					{
						const double angle = m_startDragPos.Dot(finalIntersection) / (m_startDragPos.Magnitude() * finalIntersection.Magnitude());
						float angleDeg = static_cast<float>(acos(angle) * 180.0 / Math::PI);
						if (!std::isnan(angleDeg))
						{
							float crossProduct = 0;

							if (m_side == Side::Side_Right)
								crossProduct = m_startDragPos.z * finalIntersection.y - m_startDragPos.y * finalIntersection.z;
							else if (m_side == Side::Side_Up)
								crossProduct = m_startDragPos.x * finalIntersection.z - m_startDragPos.z * finalIntersection.x;
							else if (m_side == Side::Side_Forward)
								crossProduct = m_startDragPos.y * finalIntersection.x - m_startDragPos.x * finalIntersection.y;

							if (m_side == Side::Side_Right)
							{
								if (objectRight.x < 0)
								{
									angleDeg *= -1;
								}
							}
							else if (m_side == Side::Side_Up)
							{
								if (objectUp.y < 0)
								{
									angleDeg *= -1;
								}
							}
							else if (m_side == Side::Side_Forward)
							{
								if (objectForward.z < 0)
								{
									angleDeg *= -1;
								}
							}

							if (crossProduct < 0)
								m_finalAngle += angleDeg;
							else
								m_finalAngle -= angleDeg;

							if (m_side == Side::Side_Right)
								selectedGoTransform->SetLocalRotation(m_startObjectRotation * Quaternion::Euler(m_finalAngle, 0, 0));
							else if (m_side == Side::Side_Up)
								selectedGoTransform->SetLocalRotation(m_startObjectRotation * Quaternion::Euler(0, m_finalAngle, 0));
							else if (m_side == Side::Side_Forward)
								selectedGoTransform->SetLocalRotation(m_startObjectRotation * Quaternion::Euler(0, 0, m_finalAngle));
						}
						m_startDragPos = finalIntersection;
					}
				}
				else if (toolMode == ToolMode::Tool_Scale)
				{
					float dotValue = 0;
					float initialDotValue = 0;
					float scaleAmount = 0;
					if (m_side == Side::Side_Right)
					{
						initialDotValue = objectRight.Dot(m_startMovePosition - selectedGoTransform->GetPosition());
						dotValue = objectRight.Dot(closestPoint - selectedGoTransform->GetPosition());
					}
					else if (m_side == Side::Side_Up)
					{
						initialDotValue = objectUp.Dot(m_startMovePosition - selectedGoTransform->GetPosition());
						dotValue = objectUp.Dot(closestPoint - selectedGoTransform->GetPosition());
					}
					else if (m_side == Side::Side_Forward)
					{
						initialDotValue = objectForward.Dot(m_startMovePosition - selectedGoTransform->GetPosition());
						dotValue = objectForward.Dot(closestPoint - selectedGoTransform->GetPosition());
					}

					scaleAmount = dotValue - initialDotValue;

					if (m_side == Side::Side_Right)
						selectedGoTransform->SetLocalScale(m_startObjectValue + Vector3(scaleAmount, 0, 0));
					else if (m_side == Side::Side_Up)
						selectedGoTransform->SetLocalScale(m_startObjectValue + Vector3(0, scaleAmount, 0));
					else if (m_side == Side::Side_Forward)
						selectedGoTransform->SetLocalScale(m_startObjectValue + Vector3(0, 0, scaleAmount));
				}
			}

			if (InputSystem::GetKeyUp(KeyCode::MOUSE_LEFT))
			{
				if (m_side != Side::Side_None)
				{
					if (toolMode == ToolMode::Tool_Move)
					{
						const std::shared_ptr<RectTransform> rect = selectedGO->GetComponent<RectTransform>();
						if (rect)
						{
							auto command = std::make_shared<InspectorRectTransformSetPositionCommand>(rect->GetUniqueId(), rect->position, m_oldRectTransformPosition);
							CommandManager::AddCommandAndExecute(command);
						}
						else 
						{
							auto command = std::make_shared<InspectorTransformSetPositionCommand>(selectedGoTransform->GetGameObject()->GetUniqueId(), selectedGoTransform->GetPosition(), m_oldTransformPosition, false);
							CommandManager::AddCommandAndExecute(command);
						}
					}
					else if (toolMode == ToolMode::Tool_Rotate)
					{
						auto command = std::make_shared<InspectorTransformSetRotationCommand>(selectedGoTransform->GetGameObject()->GetUniqueId(), selectedGoTransform->GetLocalEulerAngles(), m_oldTransformRotation, true);
						CommandManager::AddCommandAndExecute(command);
					}
					else if (toolMode == ToolMode::Tool_Scale)
					{
						auto command = std::make_shared<InspectorTransformSetLocalScaleCommand>(selectedGoTransform->GetGameObject()->GetUniqueId(), selectedGoTransform->GetLocalScale(), m_oldTransformScale);
						CommandManager::AddCommandAndExecute(command);
					}
				}
				m_side = Side::Side_None;
			}
		}
	}

	if (InputSystem::GetKeyDown(KeyCode::MOUSE_LEFT))
	{
		if (m_side == Side::Side_None)
		{
			if (InputSystem::GetKey(KeyCode::LEFT_CONTROL) && newGameObjectSelected)
			{
				Editor::AddSelectedGameObject(newGameObjectSelected);
				Editor::SetSelectedFileReference(nullptr);
			}
			else if (allowDeselection)
			{
				Editor::SetSelectedGameObject(newGameObjectSelected);
			}
		}
	}
}

Vector3 SceneMenu::GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2)
{
	Vector3 ClosestPoint;
	if (lineDir2 != Vector3(0))
	{
		const Vector3 V = linePos2 - linePos1;

		const double dotD1D2 = Vector3::Dot(lineDir1, lineDir2);

		const double a = Vector3::Dot(lineDir1, V);
		const double b = Vector3::Dot(lineDir2, V);
		const double c = Vector3::Dot(lineDir1, lineDir2);

		const double s = (a - b * dotD1D2) / (1 - dotD1D2 * dotD1D2);
		const float t = static_cast<float>(b - c * s);

		ClosestPoint = linePos2 + t * (lineDir2 * -1);
	}
	return ClosestPoint;
}

void SceneMenu::Switch2DMode(bool is2D)
{
	m_mode2D = is2D;
	if (m_mode2D)
	{
		weakCamera.lock()->SetProjectionType(ProjectionType::Orthographic);
		gridAxis = 2;
	}
	else
	{
		weakCamera.lock()->SetProjectionType(ProjectionType::Perspective);
		gridAxis = 0;
	}
}

void SceneMenu::FocusSelectedObject()
{
	if (Editor::GetSelectedGameObjects().size() != 1)
		return;

	if (auto selectedGameObject = Editor::GetSelectedGameObjects()[0].lock())
	{
		std::shared_ptr<Transform> cameraTransform = weakCamera.lock()->GetTransform();
		std::shared_ptr<Transform> selectedObjectTransform = selectedGameObject->GetTransform();
		const Vector3 dir = (cameraTransform->GetPosition() - selectedObjectTransform->GetPosition()).Normalized();
		cameraTransform->SetPosition(selectedObjectTransform->GetPosition() + dir * 2);
		cameraTransform->SetEulerAngles(Vector3::LookAt(cameraTransform->GetPosition(), selectedObjectTransform->GetPosition()));
	}
}

void SceneMenu::Draw()
{
	std::shared_ptr<Camera> camera = weakCamera.lock();
	Vector2Int frameBufferSize = Vector2Int(0, 0);
	if (camera)
	{
		frameBufferSize.x = camera->GetWidth();
		frameBufferSize.y = camera->GetHeight();
	}

	// Generate tab name
	std::string windowName = "Scene";
	if (m_isLastFrameOpened)
		windowName += " " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y);
	windowName += "###Scene" + std::to_string(id);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	const bool visible = ImGui::Begin(windowName.c_str(), &m_isActive, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoCollapse);
	m_isLastFrameOpened = visible;
	if (visible)
	{
		OnStartDrawing();
		bool canProcess = false;

		const ImVec2 startCursorPos = ImGui::GetCursorPos();
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
		{
			ImGui::SetWindowFocus();
			m_isFocused = true;
		}
		if (camera && (m_startAvailableSize.x != 0 && m_startAvailableSize.y != 0))
		{
			MoveCamera();

			camera->ChangeFrameBufferSize(Vector2Int(static_cast<int>(m_startAvailableSize.x), static_cast<int>(m_startAvailableSize.y)));
			ImGui::Image((ImTextureID)(size_t)camera->m_secondFramebufferTexture, ImVec2(m_startAvailableSize.x, m_startAvailableSize.y), ImVec2(0, 1), ImVec2(1, 0));

			std::shared_ptr<FileReference> mesh = nullptr;
			EditorUI::DragDropTarget("Files" + std::to_string((int)FileType::File_Mesh), mesh, false);
			std::shared_ptr<FileReference> prefabFileRef = nullptr;
			EditorUI::DragDropTarget("Files" + std::to_string((int)FileType::File_Prefab), prefabFileRef, false);
			if (mesh || prefabFileRef)
			{
				Vector3 worldCoords;
				Vector3 mouseWorldDir;
				Vector3 mouseWorldDirNormalized;
				GetMouseRay(mouseWorldDir, mouseWorldDirNormalized, worldCoords, *camera);

				if (m_draggedMeshGameObject)
				{
					m_draggedMeshGameObject->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + mouseWorldDirNormalized * -6);
				}
				else
				{
					if (mesh)
					{
						std::shared_ptr<GameObject> newGameObject = CreateGameObject(mesh->m_file->GetFileName());

						newGameObject->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + mouseWorldDirNormalized * -6);
						std::shared_ptr<MeshRenderer> meshRenderer = newGameObject->AddComponent<MeshRenderer>();
						meshRenderer->SetMeshData(std::dynamic_pointer_cast<MeshData>(mesh));
						const size_t matCount = meshRenderer->GetMaterials().size();
						for (int i = 0; i < matCount; i++)
						{
							meshRenderer->SetMaterial(AssetManager::standardMaterial, i);
						}
						Editor::SetSelectedGameObject(newGameObject);
						SceneManager::SetIsSceneDirty(true);
						m_draggedMeshGameObject = newGameObject;
					}
					else 
					{
						// Instantiate prefab
						std::shared_ptr<Prefab> prefab = std::dynamic_pointer_cast<Prefab>(prefabFileRef);
						if (prefab)
						{
							std::shared_ptr<GameObject> newGameObject = nullptr;
							SceneManager::CreateObjectsFromJson(prefab->GetData(), true, &newGameObject);
							if (newGameObject)
							{
								newGameObject->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + mouseWorldDirNormalized * -6);
								Editor::SetSelectedGameObject(newGameObject);
								SceneManager::SetIsSceneDirty(true);
								m_draggedMeshGameObject = newGameObject;
							}
							else 
							{
								Debug::PrintError("Corrupted prefab! " + prefabFileRef->m_file->GetPath());
							}
						}
					}
				}
			}
			else
			{
				m_draggedMeshGameObject.reset();
			}

			if (ImGui::IsItemHovered())
			{
				if (InputSystem::GetKey(KeyCode::LEFT_CONTROL) && InputSystem::GetKeyDown(KeyCode::F))
				{
					if (Editor::GetSelectedGameObjects().size() == 1)
					{
						if (std::shared_ptr<GameObject> selectedGameObject = Editor::GetSelectedGameObjects()[0].lock())
						{
							selectedGameObject->GetTransform()->SetPosition(camera->GetTransform()->GetPosition() + camera->GetTransform()->GetForward() * 2);
						}
					}
				}
				else if (InputSystem::GetKeyDown(KeyCode::F))
				{
					FocusSelectedObject();
				}

				canProcess = true;
			}
		}

		// List tool modes
		ImGui::SetCursorPos(startCursorPos);
		const bool toolButtonClicked = DrawToolWindow();
		if (canProcess)
		{
			ProcessTool(camera, !toolButtonClicked);
		}
		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
	ImGui::PopStyleVar();
}

bool SceneMenu::DrawImageButton(bool enabled, const Texture& texture, const std::string& buttonId, bool& isHovered)
{
	if (!enabled)
	{
		ImGui::BeginDisabled();
	}

	const bool clicked = ImGui::ImageButton(buttonId.c_str(), (ImTextureID)(size_t)EditorUI::GetTextureId(texture), ImVec2(24 * GetUIScale(), 24 * GetUIScale()), ImVec2(0.005f, 0.005f), ImVec2(0.995f, 0.995f));
	if (!enabled)
	{
		ImGui::EndDisabled();
	}

	if (ImGui::IsItemHovered())
	{
		isHovered = true;
	}

	return clicked;
}

bool SceneMenu::DrawToolWindow()
{
	bool buttonHovered = false;
	std::string toolModeText = "Tool mode (Camera speed: " + std::to_string(static_cast<int>(Editor::s_cameraSpeed)) + ")";
	if (ImGui::CollapsingHeader(toolModeText.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		EditorUI::SetButtonColor(toolMode == ToolMode::Tool_MoveCamera);
		const bool moveCameraClicked = DrawImageButton(true, *EditorIcons::GetIcons()[static_cast<int>(IconName::Icon_Camera_Move)], "##SceneMoveCameraButton", buttonHovered);
		EditorUI::EndButtonColor();

		EditorUI::SetButtonColor(toolMode == ToolMode::Tool_Move);
		const bool moveClicked = DrawImageButton(true, *EditorIcons::GetIcons()[(int)IconName::Icon_Move], "##SceneMoveButton", buttonHovered);
		EditorUI::EndButtonColor();
		EditorUI::SetButtonColor(toolMode == ToolMode::Tool_Rotate);
		const bool rotateClicked = DrawImageButton(true, *EditorIcons::GetIcons()[(int)IconName::Icon_Rotate], "##SceneRotateButton", buttonHovered);
		EditorUI::EndButtonColor();
		EditorUI::SetButtonColor(toolMode == ToolMode::Tool_Scale);
		const bool scaleClicked = DrawImageButton(true, *EditorIcons::GetIcons()[(int)IconName::Icon_Scale], "##SceneScaleButton", buttonHovered);
		EditorUI::EndButtonColor();
		EditorUI::SetButtonColor(Graphics::IsGridRenderingEnabled());
		const bool gridClicked = DrawImageButton(true, *EditorIcons::GetIcons()[(int)IconName::Icon_Grid], "##SceneGridButton", buttonHovered);
		EditorUI::EndButtonColor();

		if (moveCameraClicked)
		{
			toolMode = ToolMode::Tool_MoveCamera;
		}
		else if (moveClicked)
		{
			toolMode = ToolMode::Tool_Move;
		}
		else if (rotateClicked)
		{
			toolMode = ToolMode::Tool_Rotate;
			Editor::s_isToolLocalMode = false;
		}
		else if (scaleClicked)
		{
			toolMode = ToolMode::Tool_Scale;
			Editor::s_isToolLocalMode = false;
		}

		if (gridClicked)
		{
			Graphics::SetIsGridRenderingEnabled(!Graphics::IsGridRenderingEnabled());
		}

		EditorUI::SetButtonColor(m_mode2D);
		if (ImGui::Button("2D"))
		{
			Switch2DMode(!m_mode2D);
		}
		if (ImGui::IsItemHovered())
		{
			buttonHovered = true;
		}
		EditorUI::EndButtonColor();
		EditorUI::SetButtonColor(false);
		if (toolMode == ToolMode::Tool_Move)
		{
			if (Editor::s_isToolLocalMode)
			{
				if (ImGui::Button("Local"))
				{
					Editor::s_isToolLocalMode = false;
				}
			}
			else
			{
				if (ImGui::Button("World"))
				{
					Editor::s_isToolLocalMode = true;
				}
			}
			if (ImGui::IsItemHovered())
			{
				buttonHovered = true;
			}
		}
		EditorUI::EndButtonColor();
	}

	return buttonHovered;
}
