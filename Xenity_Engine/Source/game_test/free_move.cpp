#include "free_move.h"
#include "../xenity.h"

FreeMove::FreeMove()
{
	componentName = "FreeMove";
}

void FreeMove::Start()
{
}

void FreeMove::Update()
{
	auto cameraTrans = GetTransform();
	Vector3 rot = cameraTrans->GetRotation();
	Vector3 pos = cameraTrans->GetPosition();

	float fwd = 0;
	float side = 0;

#if defined(__PSP__)
	if (InputSystem::GetKey(TRIANGLE))
		rot.x += -1 * Time::GetDeltaTime() * 60;
	else if (InputSystem::GetKey(CROSS))
		rot.x += 1 * Time::GetDeltaTime() * 60;

	if (InputSystem::GetKey(CIRCLE))
		rot.y += 1 * Time::GetDeltaTime() * 60;
	else if (InputSystem::GetKey(SQUARE))
		rot.y += -1 * Time::GetDeltaTime() * 60;

	fwd = InputSystem::leftJoystick.y;
	side = InputSystem::leftJoystick.x;

#elif defined(__vita__)
	fwd = InputSystem::leftJoystick.y;
	side = InputSystem::leftJoystick.x;

	rot.x += InputSystem::rightJoystick.y * Time::GetDeltaTime() * 60;
	rot.y += InputSystem::rightJoystick.x * Time::GetDeltaTime() * 60;
#endif


	pos -= cameraTrans->GetForward() * (fwd / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraTrans->GetLeft() * (side / 7.0f) * Time::GetDeltaTime() * 30;

	cameraTrans->SetPosition(pos);
	cameraTrans->SetRotation(rot);
}

std::unordered_map<std::string, ReflectionEntry> FreeMove::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	return reflectedVariables;
}

