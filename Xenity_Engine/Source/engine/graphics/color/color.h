#pragma once

#include "../../vectors/vector4.h"

class Color
{
public:
	void SetFromRGBA(int r, int g, int b, int a);
	void SetFromRGBAfloat(float r, float g, float b, float a);
	Vector4 GetRGBAfloat();

private:
	Vector4 rgba = Vector4(1, 1, 1, 1);
};

