#pragma once

#include "../vectors/vector3.h";
#include "../component.h"

class lighting
{
};
const float lightConstant = 1;

class Light : public Component
{
public:
	enum LightType
	{
		Directional,
		Point,
		Spot,
	};
	LightType type = Directional;
	Vector3 color = Vector3();
	float intensity = 0;
private:
	float range = 0;
};

class PointLight : public Light
{
public:
	//Spot and point light
	float linear = 0;
	float quadratic = 0;
	void UpdatePointLightValues();
	Vector3 position = Vector3();
	void SetRange(float value);
	float GetRange();
private:
	float range = 0;
};

class DirectionalLight : public Light
{
public:
	Vector3 position = Vector3();
	Vector3 direction = Vector3();
};

class SpotLight : public Light
{
public:
	//Spot and point light
	float linear = 0;
	float quadratic = 0;
	//For spot light
	float cutOff = 0;
	void UpdatePointLightValues();
	float outerCutOff = 0;
	void SetRange(float value);
	float GetRange();
private:
	float range = 0;
};