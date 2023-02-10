#include "lighting.h"

#include <corecrt_math.h>

/// <summary>
/// Update lights intern values
/// </summary>
void Light::UpdateLightValues()
{
	linear= (0.7f * 7.0f) / range;
	quadratic = (7 * 1.8f) / (powf(range, 2) / 6.0f);
}

/// <summary>
/// Get light range
/// </summary>
/// <returns></returns>
float Light::GetRange()
{
	return range;
}

/// <summary>
/// Set spot angle
/// </summary>
/// <param name="angle"></param>
void Light::SetSpotAngle(float angle)
{
	if (angle < 0)
		angle = 0;
	else if (angle > 179)
		angle = 179;

	spotAngle = angle;
}

/// <summary>
/// Set spot light smoothness [0;1]
/// </summary>
/// <param name="smoothness"></param>
void Light::SetSpotSmoothness(float smoothness)
{
	if (smoothness < 0)
		smoothness = 0;
	else if (smoothness > 1)
		smoothness = 1;

	spotSmoothness = smoothness;
}

/// <summary>
/// Get spot light angle [0;179]
/// </summary>
/// <returns></returns>
float Light::GetSpotAngle()
{
	return spotAngle;
}

/// <summary>
/// Get spot light smoothness
/// </summary>
/// <returns></returns>
float Light::GetSpotSmoothness()
{
	return spotSmoothness;
}

/// <summary>
/// Set light range
/// </summary>
/// <param name="value"></param>
void Light::SetRange(float value)
{
	range = value;
	UpdateLightValues();
}