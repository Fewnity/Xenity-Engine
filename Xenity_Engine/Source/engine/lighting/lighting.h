#pragma once

#include "../vectors/vector3.h"
#include "../component.h"
#include "../graphics/color/color.h"

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
	Color color = Color();
	float intensity = 0;

	//Spot and point light
	float linear = 0;
	float quadratic = 0;

	void SetupPointLight(const Color color, float intensity, const float range);
	void SetupDirectionalLight(const Color color, const float intensity);
	void SetupSpotLight(const Color color, const float intensity, const float range, const float angle);
	void SetupSpotLight(const Color color, const float intensity, const float range, const float angle, const float smoothness);

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