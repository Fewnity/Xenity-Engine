#pragma once
#include <engine/api.h>

#include <engine/vectors/vector3.h>
#include <engine/component.h>
#include <engine/graphics/color/color.h>

const float lightConstant = 1;

ENUM(LightType, Directional, Point, Spot);

class API Light : public Component
{
public:


	Light();
	~Light();

	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	/**
	* Setup the light as a point light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	*/
	void SetupPointLight(const Color& color, float _intensity, const float _range);

	/**
	* Setup the light as a directional light
	* @param color Light color
	* @param intensity Light intensity
	*/
	void SetupDirectionalLight(const Color& color, const float _intensity);

	/**
	* Setup the light as a spot light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	* @param angle Spot angle [0;179]
	*/
	void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle);

	/**
	* Setup the light as a point light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	* @param angle Spot angle [0;179]
	* @param smoothness Spot smoothness [0;1]
	*/
	void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle, const float _smoothness);

	/**
	* Set light range
	* @param value Light range (Greater or equals to 0)
	*/
	void SetRange(float value);

	/**
	* Get light range
	* @param value Light range
	*/
	float GetRange() const;

	/**
	* Set spot angle
	* @param value Spot angle [0;179]
	*/
	void SetSpotAngle(float angle);

	/**
	* Set spot smoothness
	* @param value Spot smoothness [0;1]
	*/
	void SetSpotSmoothness(float smoothness);

	/**
	* Get spot angle
	*/
	float GetSpotAngle() const;

	/**
	* Get spot smoothness
	*/
	float GetSpotSmoothness() const;

	Color color = Color();

	void SetIntensity(float intensity);
	float GetIntensity() const;


	//// Internal
	//Spot and point light
	float linear = 0;
	float quadratic = 0;

	void OnDrawGizmos() override;

private:

	float intensity = 1;

	/**
	* Calculate light values
	*/
	void UpdateLightValues();

	//Spot and point light
	float range = 10;
	//For spot light
	float spotAngle = 30;
	float spotSmoothness = 0;

public:
	LightType type = LightType::Directional;
};