#pragma once

#include "../vectors/vector3.h"
#include "../component.h"

class lighting
{
};
const float lightConstant = 1;

class Light : public Component
{
public:

	Light();
	~Light();

	enum LightType
	{
		Directional,
		Point,
		Spot,
	};

	LightType type = Directional;
	Vector3 color = Vector3();
	float intensity = 0;

	//Spot and point light
	float linear = 0;
	float quadratic = 0;

	void SetupPointLight(const Vector3 color, float intensity, const float range);
	void SetupDirectionalLight(const Vector3 color, const float intensity);
	void SetupSpotLight(const Vector3 color, const float intensity, const float range, const float angle);
	void SetupSpotLight(const Vector3 color, const float intensity, const float range, const float angle, const float smoothness);

	void UpdateLightValues();
	void SetRange(float value);
	float GetRange() const;


	void SetSpotAngle(float angle);
	void SetSpotSmoothness(float smoothness);
	float GetSpotAngle() const;
	float GetSpotSmoothness() const;

private:
	//Spot and point light
	float range = 10;
	//For spot light
	float spotAngle = 30;
	float spotSmoothness = 0;
};