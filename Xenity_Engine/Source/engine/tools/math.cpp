// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "math.h"

#if defined(__PSP__)
#include <pspgum.h>
#endif
#include <glm/gtx/quaternion.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <engine/vectors/vector3.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/quaternion.h>
#include <engine/tools/math.h>

void Math::MultiplyMatrices(const float* A, const float* B, float* result, int rA, int cA, int rB, int cB)
{
	if (cA != rB)
	{
		return;
	}

	float temp;

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

glm::mat4 Math::CreateModelMatrix(const Vector3& position, const Vector3& rotation, const Vector3& scale)
{
	glm::mat4 transformationMatrix;
#if defined(__PSP__)
	ScePspFMatrix4 pspTransformationMatrix;
	gumLoadIdentity(&pspTransformationMatrix);
	ScePspFVector3 pspPosition = { -position.x, position.y, position.z };
	gumTranslate(&pspTransformationMatrix, &pspPosition);

if (rotation.y != 0)
	gumRotateY(&pspTransformationMatrix, glm::radians(rotation.y*-1));
if (rotation.x != 0)
	gumRotateX(&pspTransformationMatrix, glm::radians(rotation.x));
if (rotation.z != 0)
	gumRotateZ(&pspTransformationMatrix, glm::radians(rotation.z*-1));

	ScePspFVector3 pspScale = { scale.x, scale.y, scale.z };
	gumScale(&pspTransformationMatrix, &pspScale);

	transformationMatrix = *((glm::mat4*)(&pspTransformationMatrix));
#else
	transformationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, position.y, position.z));
	if (rotation.y != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.y * -1), glm::vec3(0.0f, 1.0f, 0.0f));
	if (rotation.x != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	if (rotation.z != 0)
		transformationMatrix = glm::rotate(transformationMatrix, glm::radians(rotation.z * -1), glm::vec3(0.0f, 0.0f, 1.0f));
	//if (scale.x != 1 || scale.y != 1|| scale.z != 1)
	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));
#endif

	return transformationMatrix;
}

glm::mat4 Math::CreateModelMatrix(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
{
	glm::mat4 transformationMatrix;
#if defined(__PSN__NO)
	ScePspFMatrix4 pspTransformationMatrix;
	gumLoadIdentity(&pspTransformationMatrix);
	ScePspFVector3 pspPosition = { -position.x, position.y, position.z };
	gumTranslate(&pspTransformationMatrix, &pspPosition);

	glm::mat4 qm = glm::toMat4(glm::quat(rotation.w, rotation.x, -rotation.y, -rotation.z));
	gumMultMatrix(&pspTransformationMatrix, &pspTransformationMatrix, (ScePspFMatrix4*)(&qm));

	ScePspFVector3 pspScale = { scale.x, scale.y, scale.z };
	gumScale(&pspTransformationMatrix, &pspScale);

	transformationMatrix = *((glm::mat4*)(&pspTransformationMatrix));
#else

	transformationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-position.x, position.y, position.z));
	
	transformationMatrix *= glm::toMat4(glm::quat(rotation.w, rotation.x, -rotation.y, -rotation.z));

	transformationMatrix = glm::scale(transformationMatrix, glm::vec3(scale.x, scale.y, scale.z));
#endif
	return transformationMatrix;
}

unsigned int Math::nextPow2(const unsigned int value)
{
	unsigned int poweroftwo = 1;
	while (poweroftwo < value)
	{
		poweroftwo <<= 1;
	}
	return poweroftwo;
}

glm::mat4 Math::MultiplyMatrices(const glm::mat4& matA, const glm::mat4& matB)
{
	glm::mat4 newMat;
#if defined(__PSP__)
	gumMultMatrix((ScePspFMatrix4*)&newMat, ((const ScePspFMatrix4*)(&matA)), ((const ScePspFMatrix4*)(&matB)));
#else
	newMat = matA * matB;
#endif
	return newMat;
}

unsigned int Math::previousPow2(const unsigned int value)
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

Vector3 Math::Get3DDirectionFromAngles(const float angleA, const float angleB)
{
	Vector3 direction = Vector3();
	const float TempS = angleA / 180.0f * Math::PI;
	const float TempT = (180 - angleB) / 180.0f * Math::PI;

	const float cosTempT = cosf(TempT);
	const float cosTempS = cosf(TempS);
	const float SinTempS = sinf(TempS);

	direction.x = SinTempS * cosTempT;
	direction.y = -sinf(TempT);
	direction.z = cosTempS * cosTempT;

	return direction;
}

Vector2 Math::Get2DDirectionFromAngle(const float angleA)
{
	Vector2 direction = Vector2();
	const float TempS = angleA / 180.0f * Math::PI;

	const float cosTempS = cosf(TempS);
	const float SinTempS = sinf(TempS);

	direction.x = cosTempS;
	direction.y = -SinTempS;
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
