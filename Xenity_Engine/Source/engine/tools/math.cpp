#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#include "math.h"
#include "../vectors/vector3.h"


void Math::MultiplyMatrix(const float *A, const float *B, float *result, int rA, int cA, int rB, int cB)
{
	if (cA != rB)
	{
		return;
	}

	float temp = 0;

	for (int i = 0; i < rA; i++)
	{
		for (int j = 0; j < cB; j++)
		{
			temp = 0;
			for (int k = 0; k < cA; k++)
			{
				temp += A[i * cA + k] * B[k * cB + j];
			}
			result[i * cB + j] = temp;
		}
	}
}

unsigned int Math::pow2(const unsigned int value)
{
	unsigned int poweroftwo = 1;
	while (poweroftwo < value)
	{
		poweroftwo <<= 1;
	}
	return poweroftwo;
}

unsigned int Math::pow2Lower(const unsigned int value)
{
	unsigned int poweroftwo = 1;
	unsigned int lastPower = poweroftwo;
	while ((poweroftwo << 1) <= value)
	{
		poweroftwo <<= 1;
		lastPower = poweroftwo;
	}
	return lastPower;
}

Vector3 Math::GetDirectionFromAngles(const float angleA, const float angleB)
{
	Vector3 direction = Vector3();
	float TempS = angleA / 180.0f * (float)M_PI;
	float TempT = (180 - angleB) / 180.0f * (float)M_PI;

	float cosTempT = cosf(TempT);
	float cosTempS = cosf(TempS);
	float SinTempS = sinf(TempS);

	direction.x = SinTempS * cosTempT;
	direction.y = -sinf(TempT);
	direction.z = cosTempS * cosTempT;

	return direction;
}

Vector3 Math::GetDirectionFromAngle(const float angleA)
{
	Vector3 direction = Vector3();
	float TempS = angleA / 180.0f * (float)M_PI;

	float cosTempS = cosf(TempS);
	float SinTempS = sinf(TempS);

	direction.x = cosTempS;
	direction.y = 0;
	direction.z = -SinTempS;
	return direction;
}

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
