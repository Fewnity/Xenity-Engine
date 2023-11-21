#include "camera_follow.h"
#include "../xenity.h"

CameraFollow::CameraFollow()
{
	componentName = "CameraFollow";
}

void CameraFollow::Start()
{
}

void CameraFollow::Update()
{
	auto transform = GetTransform();
	auto targetTransform = cameraTarget.lock()->GetTransform();

	Vector3 pos = targetTransform->GetPosition();
	pos += offset;

	Vector3 targetPos = Vector3::Lerp(transform->GetPosition(), pos, Time::GetDeltaTime() * moveSpeed);

	transform->SetPosition(targetPos);
}

ReflectiveData CameraFollow::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, cameraTarget, "cameraTarget", true);
	Reflective::AddVariable(reflectedVariables, offset, "offset", true);
	Reflective::AddVariable(reflectedVariables, moveSpeed, "moveSpeed", true);
	return reflectedVariables;
}
