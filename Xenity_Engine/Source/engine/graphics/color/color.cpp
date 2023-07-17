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
	SetReflection();
}

void Color::SetReflection()
{
	reflectedVariables["rgba"] = &rgba;
}

Color Color::CreateFromRGB(int r, int g, int b, int a)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, 255);
	return color;
}

/**
 * @brief Create color from RGB float
 *
 * @param r Red [0.0f;1.0f]
 * @param g Green [0.0f;1.0f]
 * @param b Blue [0.0f;1.0f]
 * @return Color
 */
Color Color::CreateFromRGBFloat(float r, float g, float b, float a)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, 1);
	return color;
}

/**
 * @brief Create color from RGBA
 *
 * @param r Red [0;255]
 * @param g Green [0;255]
 * @param b Blue [0;255]
 * @param a Alpha [0;255]
 * @return Color
 */
Color Color::CreateFromRGBA(int r, int g, int b, int a)
{
	Color color = Color();
	color.SetFromRGBA(r, g, b, a);
	return color;
}

/**
 * @brief Create color from RGBA float
 *
 * @param r Red [0.0f;1.0f]
 * @param g Green [0.0f;1.0f]
 * @param b Blue [0.0f;1.0f]
 * @param a Alpha [0.0f;1.0f]
 * @return Color
 */
Color Color::CreateFromRGBAFloat(float r, float g, float b, float a)
{
	Color color = Color();
	color.SetFromRGBAfloat(r, g, b, a);
	return color;
}

/**
 * @brief Set color from RGBA
 *
 * @param r Red [0;255]
 * @param g Green [0;255]
 * @param b Blue [0;255]
 * @param a Alpha [0;255]
 */
void Color::SetFromRGBA(int r, int g, int b, int a)
{
	rgba.r = Math::Clamp(r / 255.0f, 0, 1);
	rgba.g = Math::Clamp(g / 255.0f, 0, 1);
	rgba.b = Math::Clamp(b / 255.0f, 0, 1);
	rgba.a = Math::Clamp(a / 255.0f, 0, 1);
	rgbaInt = ((int)(rgba.r * 255) << 24) + ((int)(rgba.g * 255) << 16) + ((int)(rgba.b * 255) << 8) + ((int)(rgba.a * 255) << 0);
	abgrInt = ((int)(rgba.a * 255) << 24) + ((int)(rgba.b * 255) << 16) + ((int)(rgba.g * 255) << 8) + ((int)(rgba.r * 255) << 0);
}

/**
 * @brief Set color from RGBA float
 *
 * @param r Red [0.0f;1.0f]
 * @param g Green [0.0f;1.0f]
 * @param b Blue [0.0f;1.0f]
 * @param a Alpha [0.0f;1.0f]
 */
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

/**
 * @brief
 *
 * @return RGBA
 */
RGBA Color::GetRGBA()
{
	return rgba;
}

void RGBA::SetReflection()
{
	reflectedVariables["r"] = &r;
	reflectedVariables["g"] = &g;
	reflectedVariables["b"] = &b;
	reflectedVariables["a"] = &a;
}

/**
 * @brief
 *
 * @return Vector4
 */
Vector4 RGBA::ToVector4()
{
	return Vector4(r, g, b, a);
}