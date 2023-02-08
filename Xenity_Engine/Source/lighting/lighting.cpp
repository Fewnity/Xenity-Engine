#include "lighting.h"

#include <corecrt_math.h>


void Light::UpdateLightValues()
{
	linear= (0.7f * 7.0f) / range;
	quadratic = (7 * 1.8f) / (powf(range, 2) / 6.0f);
}

float Light::GetRange()
{
	return range;
}

void Light::SetSpotAngle(float angle)
{
	if (angle < 0)
		angle = 0;
	else if (angle > 179)
		angle = 179;

	spotAngle = angle;
}

void Light::SetSpotSmoothness(float smoothness)
{
	if (smoothness < 0)
		smoothness = 0;
	else if (smoothness > 1)
		smoothness = 1;

	spotSmoothness = smoothness;
}

float Light::GetSpotAngle()
{
	return spotAngle;
}

float Light::GetSpotSmoothness()
{
	return spotSmoothness;
}

void Light::SetRange(float value)
{
	range = value;
	UpdateLightValues();
}