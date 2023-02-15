#include "camera.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "graphics.h"

Camera::Camera() {
	this->fov = 60;
	if (Graphics::usedCamera == nullptr) 
	{
		Graphics::usedCamera = this;
	}
}

Camera::~Camera()
{
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
	Vector3 vect = Vector3();
	if (gameObject != nullptr) {
		float TempS = this->gameObject->transform.GetRotation().y / 180.0f * (float)M_PI;
		float TempT = (180 - this->gameObject->transform.GetRotation().x) / 180.0f * (float)M_PI;

		float cosTempT = cosf(TempT);
		float cosTempS = cosf(TempS);
		float SinTempS = sinf(TempS);

		vect.x = SinTempS * cosTempT;
		vect.y = -sinf(TempT);
		vect.z = cosTempS * cosTempT;
	}
	return vect;
}

/// <summary>
/// TODO Optimize this, (add a boolean to check if we need to re calculate values)
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <param name="z"></param>
Vector3 Camera::GetSphericalCoordinate2() {
	Vector3 vect = Vector3();
	if (gameObject != nullptr) {
		float TempS = this->gameObject->transform.GetRotation().y / 180.0f * (float)M_PI;

		float cosTempS = cosf(TempS);
		float SinTempS = sinf(TempS);

		vect.x = cosTempS;
		vect.y = 0;
		vect.z = -SinTempS;
	}
	return vect;
}