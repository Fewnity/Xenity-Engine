#pragma once

#include "../../vectors/vector4.h"

class RGBA 
{
public:
	RGBA(float r, float g, float b, float a) 
	{
		this->r = r;
		this->a = a;
		this->b = b;
		this->a = a;
	}
	Vector4 ToVector4();

	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;
};

class Color
{
public:
	static Color CreateFromRGBA(int r, int g, int b, int a);
	static Color CreateFromRGBAFloat(float r, float g, float b, float a);
	void SetFromRGBA(int r, int g, int b, int a);
	void SetFromRGBAfloat(float r, float g, float b, float a);
	RGBA GetRGBA();

private:
	RGBA rgba = RGBA(1, 1, 1, 1);
};

