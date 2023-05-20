#include "camera_manager.h"
#include "../../xenity.h"
#include <string>

CameraManager::CameraManager()
{
	reflectedFloats.insert(std::pair<std::string, float*>("Camera Zoom", &cameraZoom));
	reflectedFloats.insert(std::pair<std::string, float*>("Camera Arrow Move Speed", &cameraArrowMoveSpeed));
	componentName = "Camera Manager";
}

void CameraManager::Awake()
{

}

void CameraManager::Init() 
{
	GameObject* cameraGameObject = new GameObject("Camera");
	camera = cameraGameObject->AddComponent<Camera>();

	camera->GetTransform()->SetPosition(Vector3(0, 0, 0));

	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(2.5f * cameraZoom);
}

void CameraManager::Update()
{
	MoveCamera();
	ZoomCamera();
}

void CameraManager::MoveCamera()
{
	Vector3 newCameraPosition = camera->GetTransform()->GetPosition();

	if (InputSystem::GetKey(MOUSE_RIGHT))
	{
		Vector3 vect = Graphics::usedCamera->GetTransform()->GetDown();
		vect *= InputSystem::mouseSpeed.y * 14.2f * cameraZoom / 2.8f;
		newCameraPosition += vect;

		vect = Graphics::usedCamera->GetTransform()->GetLeft();
		vect *= InputSystem::mouseSpeed.x * 14.2f * cameraZoom / 2.8f;
		newCameraPosition += vect;
	}

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetTransform()->GetUp();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = Graphics::usedCamera->GetTransform()->GetDown();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = Graphics::usedCamera->GetTransform()->GetRight();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{
		Vector3 vect = Graphics::usedCamera->GetTransform()->GetLeft();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}

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

void CameraManager::ZoomCamera()
{
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