#include "color.h"
#include "../../tools/math.h"
#include <math.h>

Color Color::CreateFromRGBA(int r, int g, int b, int a)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, a);
	return color;
}

Color Color::CreateFromRGBAFloat(float r, float g, float b, float a)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, a);
	return color;
}

void Color::SetFromRGBA(int r, int g, int b, int a)
{
	rgba.r = (int)round(Math::Clamp(r / 255.0f, 0, 1));
	rgba.g = (int)round(Math::Clamp(g / 255.0f, 0, 1));
	rgba.b = (int)round(Math::Clamp(b / 255.0f, 0, 1));
	rgba.a = (int)round(Math::Clamp(a / 255.0f, 0, 1));
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

void Color::SetFromRGBAfloat(float r, float g, float b, float a)
{
	rgba.r = Math::Clamp(r, 0, 1);
	rgba.g = Math::Clamp(g, 0, 1);
	rgba.b = Math::Clamp(b, 0, 1);
	rgba.a = Math::Clamp(a, 0, 1);
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

unsigned int Color::GetUnsignedIntRGBA()
{
	return rgbaInt;
}

unsigned int Color::GetUnsignedIntABGR()
{
	return abgrInt;
}

RGBA Color::GetRGBA()
{
	return rgba;
}

Vector4 RGBA::ToVector4()
{
	return Vector4(r, g, b, a);
}