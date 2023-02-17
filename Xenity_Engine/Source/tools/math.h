#pragma once

class Vector3;

class Math 
{
public:
	static void MultiplyMatrix(const double* A, const double* B, double* result, int rA, int cA, int rB, int cB);
	static Vector3 GetDirectionFromAngles(const float angleA, const float angleB);
	static Vector3 GetDirectionFromAngle(const float angleA);
};