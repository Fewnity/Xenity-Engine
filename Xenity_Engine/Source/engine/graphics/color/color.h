#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../../vectors/vector4.h"
#include "../../reflection/reflection.h"

class API RGBA : public Reflection
{
public:
	//void SetReflection();
	std::unordered_map<std::string, Variable> GetReflection();
	RGBA() 
	{
		//SetReflection();
	}

	RGBA(float r, float g, float b, float a)
	{
		this->r = r;
		this->a = a;
		this->b = b;
		this->a = a;
		//SetReflection();
	}
	Vector4 ToVector4();

	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;
};

class API Color : public Reflection
{
public:
	Color();
	//void SetReflection();
	std::unordered_map<std::string, Variable> GetReflection();
	static Color CreateFromRGB(int r, int g, int b);
	static Color CreateFromRGBFloat(float r, float g, float b);
	static Color CreateFromRGBA(int r, int g, int b, int a);
	static Color CreateFromRGBAFloat(float r, float g, float b, float a);
	void SetFromRGBA(int r, int g, int b, int a);
	void SetFromRGBAfloat(float r, float g, float b, float a);
	RGBA GetRGBA();
	unsigned int GetUnsignedIntRGBA();
	unsigned int GetUnsignedIntABGR();

private:
	RGBA rgba = RGBA(1, 1, 1, 1);
	unsigned int rgbaInt = 0xFFFFFFFF;
	unsigned int abgrInt = 0xFFFFFFFF;
};
