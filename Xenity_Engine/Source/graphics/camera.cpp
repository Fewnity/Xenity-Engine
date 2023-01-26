#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>

Camera::Camera() {
	this->fov = 60;
}

void Camera::SetFov(double fov)
{
	this->fov = fov;
}

double Camera::GetFov()
{
	return this->fov;
}

/// <summary>
/// TODO Optimize this, (add a boolean to check if we need to re calculate values)
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
Vector3 Camera::GetSphericalCoordinate() {
	float TempS = this->gameObject->transform.rotation.y / 180.0f * (float)M_PI;
	float TempT = (180 - this->gameObject->transform.rotation.x) / 180.0f * (float)M_PI;

	float cosTempT = cosf(TempT);
	float cosTempS = cosf(TempS);
	float SinTempS = sinf(TempS);

	Vector3 vect = Vector3();
	vect.x = SinTempS * cosTempT;
	vect.y = -sinf(TempT);
	vect.z = cosTempS * cosTempT;
	return vect;
}

/// <summary>
/// TODO Optimize this, (add a boolean to check if we need to re calculate values)
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
Vector3 Camera::GetSphericalCoordinate2() {
	float TempS = this->gameObject->transform.rotation.y / 180.0f * (float)M_PI;

	float cosTempS = cosf(TempS);
	float SinTempS = sinf(TempS);

	Vector3 vect = Vector3();
	vect.x = cosTempS;
	vect.y = 0;
	vect.z = -SinTempS;

	return vect;
}