#include "color.h"
#include "../../tools/math.h"
#include <math.h>
#include "../../debug/debug.h"

/**
 * @brief Create color from RGB
 *
 * @param r Red [0;255]
 * @param g Green [0;255]
 * @param b Blue [0;255]
 * @return Color
 */

Color::Color()
{
	//SetReflection();
}

/*void Color::SetReflection()
{
	reflectedVariables["rgba"] = rgba;
}*/

std::unordered_map<std::string, ReflectionEntry> Color::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, rgba, "rgba", true);
	return reflectedVariables;
}

void Color::OnReflectionUpdated()
{
	Color::UpdateUnsignedInts();
}

Color Color::CreateFromRGB(int r, int g, int b)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, 255);
	return color;
}

Color Color::CreateFromRGBFloat(float r, float g, float b)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, 1.0f);
	return color;
}

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
	rgba.r = Math::Clamp(r / 255.0f, 0, 1);
	rgba.g = Math::Clamp(g / 255.0f, 0, 1);
	rgba.b = Math::Clamp(b / 255.0f, 0, 1);
	rgba.a = Math::Clamp(a / 255.0f, 0, 1);
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

void Color::UpdateUnsignedInts()
{
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

/*void RGBA::SetReflection()
{
	reflectedVariables["r"] = r;
	reflectedVariables["g"] = g;
	reflectedVariables["b"] = b;
	reflectedVariables["a"] = a;
}*/

std::unordered_map<std::string, ReflectionEntry> RGBA::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, r, "r", true);
	Reflection::AddReflectionVariable(reflectedVariables, g, "g", true);
	Reflection::AddReflectionVariable(reflectedVariables, b, "b", true);
	Reflection::AddReflectionVariable(reflectedVariables, a, "a", true);
	return reflectedVariables;
}

Vector4 RGBA::ToVector4()
{
	return Vector4(r, g, b, a);
}