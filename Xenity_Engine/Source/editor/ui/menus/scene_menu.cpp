#include "scene_menu.h"
#include <imgui/imgui.h>
#include "../../../xenity.h"
#include <glm/ext/matrix_projection.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "../../tool_mode.h"
#include "../../editor.h"

enum Side {
	Side_None,
	Side_Right,
	Side_Up,
	Side_Forward,
};

Vector3 oldWorldMousePosition;
Vector3 worldMousePosition;
Vector3 startMovePosition;
Vector3 startObjectValue;
Side side = Side_None;
float snapAmount = 1.0f;
int cameraHandMoveSpeed = 200;
ToolMode toolMode = Tool_Move;

std::weak_ptr<GameObject> cube1;
std::weak_ptr<GameObject> cube2;
std::weak_ptr<GameObject> cube3;


void SceneMenu::Init()
{
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

float Dot(const Vector3& a, const Vector3& b)
{
	float f;
	f = a.x * b.x + a.y * b.y + a.z * b.z;
	return f;
}

Vector3 CrossProduct(const Vector3& a, const Vector3& b)
{
	Vector3 c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = -(a.x * b.z - a.z * b.x);
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

Vector3 GetNearestPoint(const Vector3& linePos1, const Vector3& lineDir1, const Vector3& linePos2, const Vector3& lineDir2)
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

void SceneMenu::Draw()
{
	/*if (InputSystem::GetKeyDown(E))
	{
		cube1 = ShapeSpawner::SpawnCube();
		cube2 = ShapeSpawner::SpawnCube();
		cube3 = ShapeSpawner::SpawnCube();

		cube1.lock()->GetTransform()->SetLocalScale(Vector3(0.05));
		cube2.lock()->GetTransform()->SetLocalScale(Vector3(0.15));
		cube3.lock()->GetTransform()->SetLocalScale(Vector3(0.3));
	}*/
	//Get the editor camera
	size_t cameraCount = Graphics::cameras.size();
	std::shared_ptr<Camera> camera;
	Vector2Int frameBufferSize;
	for (size_t i = 0; i < cameraCount; i++)
	{
		if (Graphics::cameras[i].lock()->isEditor)
		{
			camera = Graphics::cameras[i].lock();
			frameBufferSize.x = camera->GetWidth();
			frameBufferSize.y = camera->GetHeight();
			break;
		}
	}

	windowSize = Vector2Int(0, 0);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	std::string windowName = "Scene " + std::to_string(frameBufferSize.x) + "x" + std::to_string(frameBufferSize.y) + "###Scene";
	bool visible = ImGui::Begin(windowName.c_str(), 0, ImGuiWindowFlags_NoNav);
	if (visible)
	{
		ImVec2 size = ImGui::GetContentRegionAvail();

		ImVec2 startCursorPos = ImGui::GetCursorPos();
		if (InputSystem::GetKeyDown(MOUSE_RIGHT) && ImGui::IsWindowHovered())
		{
			ImGui::SetWindowFocus();
		}
		if (camera)
		{
			camera->ChangeFrameBufferSize(Vector2Int((int)size.x, (int)size.y));
			ImGui::Image((ImTextureID)camera->secondFramebufferTexture, size, ImVec2(0, 1), ImVec2(1, 0));
			windowSize = Vector2Int((int)size.x, (int)size.y);
			if (ImGui::IsItemHovered())
			{
				std::shared_ptr<Transform> cameraTransform = camera->GetTransform();
				isHovered = true;

				//------------------------------------------------------------------------- Detect user inputs to apply tool's behaviour on the selected gameobject

				// Calculate camera matrix without translate
				Vector3 cameraRotation = cameraTransform->GetRotation();
				glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
				//cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
				cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
				cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));

				// Get screen mouse position (inverted)
				glm::vec3 mousePositionGLM = glm::vec3(size.x - (mousePosition.x - windowPosition.x), size.y - (windowSize.y - (mousePosition.y - windowPosition.y)), 0.0f); // Invert Y for OpenGL coordinates

				// Get world mouse position (position at the near clipping plane)
				glm::vec3 worldCoords = glm::unProject(mousePositionGLM, cameraModelMatrix, camera->projection, glm::vec4(0, 0, size.x, size.y));

				// Normalise direction if needed
				Vector3 mouseWorldDir = Vector3(worldCoords.x, worldCoords.y, worldCoords.z);
				if (toolMode != Tool_MoveCamera)
					mouseWorldDir = mouseWorldDir.Normalised();
				mouseWorldDir *= -1;

				// Store world mouse position
				oldWorldMousePosition = Vector3(worldMousePosition.x, worldMousePosition.y, worldMousePosition.z);
				worldMousePosition = Vector3(mouseWorldDir.x, mouseWorldDir.y, mouseWorldDir.z);

				if (toolMode == Tool_MoveCamera) // If the tool mode is the move camera
				{
					// Move the camera if the mouse left button is held
					if (InputSystem::GetKey(MOUSE_LEFT))
					{
						Vector3 newPos = cameraTransform->GetPosition() + (oldWorldMousePosition - worldMousePosition) * cameraHandMoveSpeed;
						cameraTransform->SetPosition(newPos);
					}
				}
				else
				{
					auto selectedGO = Editor::GetSelectedGameObject();
					if (selectedGO)
					{
						auto selectedGoTransform = selectedGO->GetTransform();

						Vector3 camPos = cameraTransform->GetPosition();
						Vector3 objectPos = selectedGoTransform->GetPosition();

						// Get selected gameObject directions
						Vector3 objectRight = selectedGoTransform->GetRight();
						Vector3 objectUp = selectedGoTransform->GetUp();
						Vector3 objectForward = selectedGoTransform->GetForward();

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
								//cube1.lock()->GetTransform()->SetPosition(rightClosestPoint);
								//cube2.lock()->GetTransform()->SetPosition(upClosestPoint);
								//cube3.lock()->GetTransform()->SetPosition(forwardClosestPoint);
							}

							// Get the distance between the gameobject and the closest point of the camera's direction
							float rightDist = Vector3::Distance(rightClosestPointCam, objectPos);
							float upDist = Vector3::Distance(upClosestPointCam, objectPos);
							float forwardDist = Vector3::Distance(forwardClosestPointCam, objectPos);

							// Get the distance between the two lines nearest points
							float rightDist2 = Vector3::Distance(rightClosestPointCam, rightClosestPoint);
							float upDist2 = Vector3::Distance(upClosestPointCam, upClosestPoint);
							float forwardDist2 = Vector3::Distance(forwardClosestPointCam, forwardClosestPoint);

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

							// Detect the arrow
							side = Side_None;
							if ((rightDist2 < upDist2 || !isUpGood) && (rightDist2 < forwardDist2 || !isForwardGood) && isRightGood && rightDist2 <= 0.01f * camDist && rightDist <= 0.13f * camDist)
							{
								side = Side_Right;
							}
							else if ((upDist2 < rightDist2 || !isRightGood) && (upDist2 < forwardDist2 || !isForwardGood) && isUpGood && upDist2 <= 0.01f * camDist && upDist <= 0.13f * camDist)
							{
								side = Side_Up;
							}
							else if ((forwardDist2 < rightDist2 || !isRightGood) && (forwardDist2 < upDist2 || !isUpGood) && isForwardGood && forwardDist2 <= 0.01f * camDist && forwardDist <= 0.13f * camDist)
							{
								side = Side_Forward;
							}

							/*if (rightDist < upDist && rightDist < forwardDist && isRightGood)
								side = 0;
							else if (upDist < rightDist && upDist < forwardDist && isUpGood)
								side = 1;
							else if (forwardDist < rightDist && forwardDist < upDist && isForwardGood)
								side = 2;
							else
								side = -1;*/

								//std::cout << side << std::endl;
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
			else
			{
				isHovered = false;
			}
		}

		// Get some values for inputs
		windowPosition = Vector2Int((int)ImGui::GetWindowPos().x, (int)ImGui::GetWindowPos().y);
		mousePosition = Vector2Int((int)ImGui::GetMousePos().x, (int)(ImGui::GetMousePos().y - (ImGui::GetWindowSize().y - size.y)));
		isFocused = ImGui::IsWindowFocused();

		// List tool modes
		ImGui::SetCursorPos(startCursorPos);
		DrawToolWindow();
	}

	ImGui::End();
	ImGui::PopStyleVar();
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
		ImGui::PopStyleColor(3);
	}
}
