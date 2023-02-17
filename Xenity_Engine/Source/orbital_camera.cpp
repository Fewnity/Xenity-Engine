#include "orbital_camera.h"

#include "graphics/camera.h"
#include "engine_settings.h"

OrbitalCamera::OrbitalCamera()
{
}

void OrbitalCamera::Update()
{
	//Vector3 cameraNewPosition = camera->gameObject->transform.GetPosition();
	//cameraNewPosition += Vector3(1)* gameObject->transform.GetForward() * EngineSettings::deltaTime;
	//gameObject->transform.SetPosition(cameraNewPosition);

	gameObject->transform.SetPosition(target->GetPosition() + target->GetForward() * -4);


	//gameObject->transform.SetPosition
}
