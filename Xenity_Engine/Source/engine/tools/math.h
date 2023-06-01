#pragma once

class Vector3;

class Math
{
public:
	static void MultiplyMatrix(const float *A, const float *B, float *result, int rA, int cA, int rB, int cB);
	static unsigned int pow2(const unsigned int value);
	static unsigned int pow2Lower(const unsigned int value);
	static Vector3 GetDirectionFromAngles(const float angleA, const float angleB);
	static Vector3 GetDirectionFromAngle(const float angleA);
	static float Lerp(float a, float b, float t);
	static float Clamp(float value, float min, float max);
};