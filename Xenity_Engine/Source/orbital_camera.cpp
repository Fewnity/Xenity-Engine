#include "orbital_camera.h"

#include "graphics/camera.h"
#include "engine_settings.h"
#include "debug/debug.h"
#include <SDL2/SDL_stdinc.h>
#include "tools/math.h"
#include "time/time.h"
#include "inputs/input_system.h"
#include "tools/math.h"

OrbitalCamera::OrbitalCamera()
{
	//Add a wall
	walls.push_back(Box(Vector3(-1, 0, 0), Vector3(1, 1, 1)));
}

// Optimized Min function
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
// Optimized Max function
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/// <summary>
/// Get the distance of a ray if the ray hits the box
/// </summary>
/// <param name="corner1">Corner1 of the box</param>
/// <param name="corner2">Corner2 of the box</param>
/// <param name="dirfrac">Direction of the ray</param>
/// <param name="startPosition">Start position of the ray</param>
/// <param name="t">Distance out</param>
/// <returns>Is hitting something</returns>
bool OrbitalCamera::GetHitDistance(Vector3 corner1, Vector3 corner2, Vector3 dirfrac, Vector3 startPosition, float* t)
{
	float t1 = (corner1.x - startPosition.x) * dirfrac.x;
	float t2 = (corner2.x - startPosition.x) * dirfrac.x;
	float t3 = (corner1.y - startPosition.y) * dirfrac.y;
	float t4 = (corner2.y - startPosition.y) * dirfrac.y;
	float t5 = (corner1.z - startPosition.z) * dirfrac.z;
	float t6 = (corner2.z - startPosition.z) * dirfrac.z;

	float tmin = MAX(MAX(MIN(t1, t2), MIN(t3, t4)), MIN(t5, t6));
	float tmax = MIN(MIN(MAX(t1, t2), MAX(t3, t4)), MAX(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us, if tmin > tmax, ray doesn't intersect AABB
	if (tmax < 0 || tmin > tmax)
	{
	}
	else
	{
		*t = tmin;
		return true;
	}
	return false;
}

/// <summary>
/// 
/// </summary>
/// <param name="distance"></param>
/// <param name="dir"></param>
/// <param name="startDir"></param>
/// <returns></returns>
bool OrbitalCamera::DetectWalls(float* distance, Vector3 dir, Vector3 startDir) {
	int boxCount = walls.size();
	float dis = 99999;
	bool foundWall = false;
	for (int i = 0; i < boxCount; i++)
	{
		Box* b = &walls[i];
		Vector3 c1 = Vector3(b->position.x - b->scale.x / 2.0f, b->position.y - b->scale.y / 2.0f, b->position.z - b->scale.z / 2.0f);
		Vector3 c2 = Vector3(b->position.x + b->scale.x / 2.0f, b->position.y + b->scale.y / 2.0f, b->position.z + b->scale.z / 2.0f);
		float t = 0;
		Vector3 frag;

		frag.x = 1.0f / dir.x;
		frag.y = 1.0f / dir.y;
		frag.z = 1.0f / dir.z;

		bool found = GetHitDistance(c1, c2, frag, startDir, &t);
		if (found)
		{
			//std::cout << "FOUND" << std::endl;
			foundWall = true;
			if (t < dis)
			{
				dis = t;
			}
		}
	}
	*distance = dis;
	return foundWall;
}

void OrbitalCamera::Update()
{
	if (InputSystem::GetKeyDown(NUM_1))
	{
		cameraStatus = OrbitalCamera::ThirdPerson;
		Debug::Print("ThirdPerson");
	}
	else if (InputSystem::GetKeyDown(NUM_2))
	{
		cameraStatus = OrbitalCamera::FirstPerson;
		Debug::Print("FirstPerson");
	}
	else if (InputSystem::GetKeyDown(NUM_3))
	{
		cameraStatus = OrbitalCamera::Camera1;
		Debug::Print("Camera1");
	}


	float ySpeed = InputSystem::mouseSpeed.y * Time::GetDeltaTime() * 20;;
	if (cameraStatus == OrbitalCamera::FirstPerson)
	{
		ySpeed = -ySpeed;
	}
	axis1 += InputSystem::mouseSpeed.x * Time::GetDeltaTime() * 20;
	axis2 += ySpeed;

	int maxAxis = 10;
	if (cameraStatus == OrbitalCamera::FirstPerson)
		maxAxis = 80;

	if (axis2 > maxAxis)
		axis2 = maxAxis;
	else if (axis2 < -80)
		axis2 = -80;

	Vector3 offset = Math::GetDirectionFromAngles(axis1, axis2);

	Vector3 FinalPos = Vector3(0);
	switch (cameraStatus)
	{
	case OrbitalCamera::ThirdPerson:
		FinalPos = UpdateThirdPersonMode(offset);
		LerpMovements(FinalPos, offset);
		break;
	case OrbitalCamera::FirstPerson:
		FinalPos = UpdateFpsMode();
		LookAtFirstPerson(FinalPos);
		break;
	case OrbitalCamera::Camera1:
		FinalPos = UpdateCamera1Mode();
		LerpMovements(FinalPos, offset);
		break;
	}

	if (speed <= 10 && Vector3::Distance(cameraWithoutOffset, FinalPos) > 0.1f)
	{
		speed += Time::GetDeltaTime() * 5;
	}
	else if (speed >= 1)
	{
		speed -= Time::GetDeltaTime() * 10;
	}
}

void OrbitalCamera::LerpMovements(Vector3 lerpTarget, Vector3 cameraOffset)
{
	cameraWithoutOffset = Vector3(Math::Lerp(cameraWithoutOffset.x, lerpTarget.x, Time::GetDeltaTime() * speed), Math::Lerp(cameraWithoutOffset.y, lerpTarget.y, Time::GetDeltaTime() * speed), Math::Lerp(cameraWithoutOffset.z, lerpTarget.z, Time::GetDeltaTime() * speed));

	if (cameraStatus == OrbitalCamera::ThirdPerson)
	{
		Vector3 LerpPos = cameraWithoutOffset + cameraOffset * currentDistance;
		gameObject->transform.SetPosition(LerpPos);
	}
	else 
	{
		gameObject->transform.SetPosition(lerpTarget);
	}

	Vector3 angles = Vector3::LookAt(gameObject->transform.GetPosition(), target->GetPosition());
	gameObject->transform.SetRotation(angles);
}

void OrbitalCamera::LookAtFirstPerson(Vector3 target) {

	Vector3 offset = Vector3(0, 0, 0);
	offset = Math::GetDirectionFromAngles(axis1, axis2);

	Vector3 angles = Vector3::LookAt(Vector3(0), offset * 5);
	gameObject->transform.SetRotation(angles);
	gameObject->transform.SetPosition(target);
}

Vector3 OrbitalCamera::UpdateFpsMode()
{
	return target->GetPosition();
}

Vector3 OrbitalCamera::UpdateThirdPersonMode(Vector3 cameraOffset)
{
	Vector3 pos = target->GetPosition();

	float dis = 0;
	DetectWalls(&dis, cameraOffset, target->GetPosition());

	if (dis < 4)
	{
		targetDistance = dis;
	}
	else
	{
		targetDistance = 4;
	}
	currentDistance = Math::Lerp(currentDistance, targetDistance, Time::GetDeltaTime() * 10);

	return pos;
}

Vector3 OrbitalCamera::UpdateCamera1Mode()
{
	return camera1->GetPosition();
}
