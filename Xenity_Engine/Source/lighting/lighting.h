#pragma once

#include "../vectors/vector3.h";
#include "../component.h"
#include "../asset_manager.h"

class lighting
{
};
const float lightConstant = 1;

class Light : public Component
{
public:

	Light()
	{
		AssetManager::AddLight(this);
	};

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

	void SetupPointLight(Vector3 color, float intensity, float range);
	void SetupDirectionalLight(Vector3 color, float intensity);
	void SetupSpotLight(Vector3 color, float intensity, float range, float angle);
	void SetupSpotLight(Vector3 color, float intensity, float range, float angle, float smoothness);

	void UpdateLightValues();
	void SetRange(float value);
	float GetRange();


	void SetSpotAngle(float angle);
	void SetSpotSmoothness(float smoothness);
	float GetSpotAngle();
	float GetSpotSmoothness();

private:
	//Spot and point light
	float range = 10;
	//For spot light
	float spotAngle = 30;
	float spotSmoothness = 0;
};