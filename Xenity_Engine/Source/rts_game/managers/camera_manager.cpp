#include "camera_manager.h"
#include "../../xenity.h"
#include <string>

CameraManager::CameraManager()
{
	reflectedFloats.insert(std::pair<std::string, float*>("Camera Zoom", &cameraZoom));
	reflectedFloats.insert(std::pair<std::string, float*>("Camera Arrow Move Speed", &cameraArrowMoveSpeed));
	componentName = "Camera Manager";
}

void CameraManager::Init() 
{
	GameObject * cameraGameObject = new GameObject("Camera");
	cameraTransform = cameraGameObject->GetTransform();
	cameraTransform->SetPosition(Vector3(0, 0, 0));
	
	//Create camera
	camera = cameraGameObject->AddComponent<Camera>();
	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(2.5f * cameraZoom);
}

void CameraManager::Update()
{
	MoveCamera();
	CheckCameraZoom();
}

void CameraManager::MoveCamera()
{
	Vector3 newCameraPosition = cameraTransform->GetPosition();

	//Move camera when using mouse
	if (InputSystem::GetKey(MOUSE_RIGHT))
	{
		Vector3 vect = cameraTransform->GetDown();
		vect *= InputSystem::mouseSpeed.y * 14.2f * cameraZoom / 2.8f;
		newCameraPosition += vect;

		vect = cameraTransform->GetLeft();
		vect *= InputSystem::mouseSpeed.x * 14.2f * cameraZoom / 2.8f;
		newCameraPosition += vect;
	}

	//Move camera when using keyboard
	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = cameraTransform->GetUp();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = cameraTransform->GetDown();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = cameraTransform->GetRight();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{
		Vector3 vect = cameraTransform->GetLeft();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}

	//Clamp camera position
	float xOffset = 0.5;
	float yOffset = 0.5;

	if (newCameraPosition.x <= 0 - xOffset)
		newCameraPosition.x = 0 - xOffset;
	else if (newCameraPosition.x >= 60 - xOffset)
		newCameraPosition.x = 60 - xOffset;

	if (newCameraPosition.y <= 0 - yOffset)
		newCameraPosition.y = 0 - yOffset;
	else if (newCameraPosition.y >= 60 - yOffset)
		newCameraPosition.y = 60 - yOffset;

	camera->GetTransform()->SetPosition(newCameraPosition);
}

/// <summary>
/// Change camera zoom
/// </summary>
void CameraManager::CheckCameraZoom()
{
	//Change zoom if the mouse wheel is used
	if (InputSystem::mouseWheel != 0)
	{
		cameraZoom -= InputSystem::mouseWheel / 3.0f;
		if (cameraZoom < 0.1)
		{
			cameraZoom = 0.1;
		}
		else if (cameraZoom > 20.8f)
		{
			cameraZoom = 20.8f;
		}

		camera->SetProjectionSize(2.5f * cameraZoom);
	}
}