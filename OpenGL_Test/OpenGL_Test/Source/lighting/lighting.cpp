#include "lighting.h"

#include <corecrt_math.h>


void SpotLight::UpdatePointLightValues()
{
	linear= (0.7f * 7.0f) / range;
	quadratic = (7 * 1.8f) / (powf(range, 2) / 6.0f);
}

void PointLight::UpdatePointLightValues()
{
	linear = (0.7f * 7.0f) / range;
	quadratic = (7 * 1.8f) / (powf(range, 2) / 6.0f);
}

float SpotLight::GetRange()
{
	return range;
}
void SpotLight::SetRange(float value)
{
	range = value;
	UpdatePointLightValues();
}


float PointLight::GetRange()
{
	return range;
}
void PointLight::SetRange(float value)
{
	range = value;
	UpdatePointLightValues();
}