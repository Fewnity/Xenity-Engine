#include "math.h"

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