#include "color.h"
#include "../../tools/math.h"
#include <math.h>

void Color::SetFromRGBA(int r, int g, int b, int a)
{
	rgba.x = (int)round(Math::Clamp(r * 255, 0, 255));
	rgba.y = (int)round(Math::Clamp(g * 255, 0, 255));
	rgba.z = (int)round(Math::Clamp(b * 255, 0, 255));
	rgba.w = (int)round(Math::Clamp(a * 255, 0, 255));
}

void Color::SetFromRGBAfloat(float r, float g, float b, float a)
{
	rgba.x = Math::Clamp(r, 0, 1);
	rgba.y = Math::Clamp(g, 0, 1);
	rgba.z = Math::Clamp(b, 0, 1);
	rgba.w = Math::Clamp(a, 0, 1);
}

Vector4 Color::GetRGBAfloat()
{
	return rgba;
}
