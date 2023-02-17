#include "math.h"
#include <SDL2/SDL_stdinc.h>
#include <corecrt_math.h>
#include "../vectors/vector3.h"

void Math::MultiplyMatrix(const double* A, const double* B, double* result, int rA, int cA, int rB, int cB)
{
	if (cA != rB)
	{
		return;
	}

	double temp = 0;

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
