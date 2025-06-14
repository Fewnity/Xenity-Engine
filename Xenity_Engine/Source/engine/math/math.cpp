#include "math.h"

float Math::Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

float Math::Clamp(float value, float min, float max)
{
	if (value < min)
	{
		value = min;
	}
	else if (value > max)
	{
		value = max;
	}
	return value;
}