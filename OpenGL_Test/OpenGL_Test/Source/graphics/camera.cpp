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
	double TempS = this->gameObject->transform.rotation.y / 180.0 * M_PI;
	double TempT = (180 - this->gameObject->transform.rotation.x) / 180.0 * M_PI;

	double cosTempT = cos(TempT);
	double cosTempS = cos(TempS);
	double SinTempS = sin(TempS);

	Vector3 vect = Vector3();
	vect.x = SinTempS * cosTempT;
	vect.y = -sin(TempT);
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
	Vector3 vect = GetSphericalCoordinate();
	float temp = vect.x;
	vect.x = -vect.z;
	vect.z = temp;

	return vect;
}