#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include "../vectors/vector3.h"
#include "../component.h"
#include "../graphics/color/color.h"

class API lighting
{
};
const float lightConstant = 1;

class API Light : public Component
{
public:

	Light();
	~Light();
	//void SetReflection();
	std::unordered_map<std::string, Variable> GetReflection();
	void OnReflectionUpdated();

	enum LightType
	{
		Directional,
		Point,
		Spot,
	};

	LightType type = Directional;
	Color color = Color();
	float intensity = 1;

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