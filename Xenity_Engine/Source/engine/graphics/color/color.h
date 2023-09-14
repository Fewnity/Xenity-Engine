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
	std::unordered_map<std::string, ReflectionEntry> GetReflection();
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
	std::unordered_map<std::string, ReflectionEntry> GetReflection();
	void OnReflectionUpdated();

	/**
	* Create color from RGB
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	*/
	static Color CreateFromRGB(int r, int g, int b);

	/**
	* Create color from float RGB
	* @param r Red level [0;1]
	* @param g Green level [0;1]
	* @param b Blue level [0;1]
	*/
	static Color CreateFromRGBFloat(float r, float g, float b);

	/**
	* Create color from RGBA
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	static Color CreateFromRGBA(int r, int g, int b, int a);

	/**
	* Create color from float RGBA
	* @param r Red level [0;1]
	* @param g Green level [0;1]
	* @param b Blue level [0;1]
	* @param a Alpha level [0;1]
	*/
	static Color CreateFromRGBAFloat(float r, float g, float b, float a);

	/**
	* Set color from RGBA
	* @param r Red level [0;255]
	* @param g Green level [0;255]
	* @param b Blue level [0;255]
	* @param a Alpha level [0;255]
	*/
	void SetFromRGBA(int r, int g, int b, int a);

	/**
	* Set color from float RGBA
	* @param r Red level [0;1]
	* @param g Green level [0;1]
	* @param b Blue level [0;1]
	* @param a Alpha level [0;1]
	*/
	void SetFromRGBAfloat(float r, float g, float b, float a);

	/**
	* Get RGBA
	*/
	RGBA GetRGBA();

	/**
	* Get RGBA value as an unsigned int
	*/
	unsigned int GetUnsignedIntRGBA();

	/**
	* Get ABGR value as an unsigned int
	*/
	unsigned int GetUnsignedIntABGR();

private:
	void UpdateUnsignedInts();

	RGBA rgba = RGBA(1, 1, 1, 1);
	unsigned int rgbaInt = 0xFFFFFFFF;
	unsigned int abgrInt = 0xFFFFFFFF;
};
