#include "unit_test_manager.h"
#include <engine/debug/debug.h>
#include <engine/graphics/color/color.h>

bool ColorConstructorTest::Start(std::string& errorOut)
{
	bool result = true;

	Color color;
	if (!Compare(color.GetUnsignedIntABGR(), (unsigned int)0xffffffff))
	{
		errorOut += "Bad Color constructor (GetUnsignedIntABGR)\n";
		result = false;
	}
	
	if (!Compare(color.GetUnsignedIntRGBA(), (unsigned int)0xffffffff))
	{
		errorOut += "Bad Color constructor (GetUnsignedIntRGBA)\n";
		result = false;
	}

	const RGBA& rgbaRef = color.GetRGBA();
	if (!Compare(rgbaRef.r, 1.0f) ||
		!Compare(rgbaRef.g, 1.0f) ||
		!Compare(rgbaRef.b, 1.0f) ||
		!Compare(rgbaRef.a, 1.0f))
	{
		errorOut += "Bad Color RGBA constructor\n";
		result = false;
	}

	return result;
}

bool ColorSetTest::Start(std::string& errorOut)
{
	bool result = true;

	const int r = 50;
	const int g = 101;
	const int b = 5;
	const int a = 200;

	const float rFloat = r / 255.0f;
	const float gFloat = g / 255.0f;
	const float bFloat = b / 255.0f;
	const float aFloat = a / 255.0f;

	Color color = Color::CreateFromRGBA(r, g, b, a);
	if (!Compare(color.GetUnsignedIntABGR(), (unsigned int)0xc8056532))
	{
		errorOut += "Bad CreateFromRGBA GetUnsignedIntABGR\n";
		result = false;
	}

	if (!Compare(color.GetUnsignedIntRGBA(), (unsigned int)0x326505c8))
	{
		errorOut += "Bad CreateFromRGBA GetUnsignedIntRGBA\n";
		result = false;
	}

	if (!Compare(color.GetRGBA().r, rFloat) ||
		!Compare(color.GetRGBA().g, gFloat) ||
		!Compare(color.GetRGBA().b, bFloat) ||
		!Compare(color.GetRGBA().a, aFloat))
	{
		errorOut += "Bad CreateFromRGBA GetRGBA\n";
		result = false;
	}

	if (!Compare(color.GetRGBA().ToVector4(), Vector4(rFloat, gFloat, bFloat, aFloat)))
	{
		errorOut += "Bad CreateFromRGBA GetRGBA ToVector4\n";
		result = false;
	}

	Color colorFloat = Color::CreateFromRGBAFloat(rFloat, gFloat, bFloat, aFloat);
	if (!Compare(colorFloat.GetUnsignedIntABGR(), (unsigned int)0xc8056532))
	{
		errorOut += "Bad CreateFromRGBAFloat GetUnsignedIntABGR\n";
		result = false;
	}

	if (!Compare(colorFloat.GetUnsignedIntRGBA(), (unsigned int)0x326505c8))
	{
		errorOut += "Bad CreateFromRGBAFloat GetUnsignedIntRGBA\n";
		result = false;
	}

	if (!Compare(colorFloat.GetRGBA().r, rFloat) ||
		!Compare(colorFloat.GetRGBA().g, gFloat) ||
		!Compare(colorFloat.GetRGBA().b, bFloat) ||
		!Compare(colorFloat.GetRGBA().a, aFloat))
	{
		errorOut += "Bad CreateFromRGBAFloat GetRGBA\n";
		result = false;
	}

	if (!Compare(colorFloat.GetRGBA().ToVector4(), Vector4(rFloat, gFloat, bFloat, aFloat)))
	{
		errorOut += "Bad CreateFromRGBAFloat GetRGBA ToVector4\n";
		result = false;
	}

	return result;
}