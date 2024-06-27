// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once
#include <engine/api.h>

#include <engine/vectors/vector3.h>
#include <engine/component.h>
#include <engine/graphics/color/color.h>

const float lightConstant = 1;

ENUM(LightType, Directional, Point, Spot, Ambient);

/**
* @brief Component to produce light
*/
class API Light : public Component
{
public:

	Light();
	~Light();

	/**
	* @brief Setup the light as a point light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	*/
	void SetupPointLight(const Color& color, float _intensity, const float _range);

	/**
	* @brief Setup the light as a directional light
	* @param color Light color
	* @param intensity Light intensity
	*/
	void SetupDirectionalLight(const Color& color, const float _intensity);

	/**
	* @brief Setup the light as a spot light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	* @param angle Spot angle [0;90]
	*/
	void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle);

	/**
	* @brief Setup the light as a point light
	* @param color Light color
	* @param intensity Light intensity
	* @param range Light Range (Greater or equals to 0)
	* @param angle Spot angle [0;90]
	* @param smoothness Spot smoothness [0;1]
	*/
	void SetupSpotLight(const Color& color, const float _intensity, const float _range, const float _angle, const float _smoothness);

	/**
	* @brief Set light range
	* @param value Light range (Greater or equals to 0)
	*/
	void SetRange(float value);

	/**
	* @brief Set spot angle
	* @param value Spot angle [0;90]
	*/
	void SetSpotAngle(float angle);

	/**
	* @brief Set spot smoothness
	* @param value Spot smoothness [0;1]
	*/
	void SetSpotSmoothness(float smoothness);

	Color color = Color();

	/**
	* @brief Set light intensity
	*/
	void SetIntensity(float intensity);

	inline LightType GetType() const
	{
		return type;
	}

	inline void SetType(LightType type)
	{
		this->type = type;
	}

	/**
	* @brief Get light range
	* @param value Light range
	*/
	inline float GetRange() const
	{
		return range;
	}

	/**
	* @brief Get spot angle
	*/
	inline float GetSpotAngle() const
	{
		return spotAngle;
	}

	/**
	* @brief Get spot smoothness
	*/
	inline float GetSpotSmoothness() const
	{
		return spotSmoothness;
	}

	/**
	* @brief Get light intensity
	*/
	inline float GetIntensity() const
	{
		return intensity;
	}

protected:

	void OnDrawGizmos() override;
	void OnDrawGizmosSelected() override;
	void RemoveReferences() override;

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	float GetLinearValue() 
	{
		return linear;
	}

	float GetQuadraticValue() 
	{
		return quadratic;
	}

	friend class Shader;
	friend class RendererOpengl;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;

	//Spot and point light
	float linear = 0;
	float quadratic = 0;

	float intensity = 1;

	/**
	* @brief Calculate light values
	*/
	void UpdateLightValues();

	//Spot and point light
	float range = 10;
	//For spot light
	float spotAngle = 30;
	float spotSmoothness = 0;

	LightType type = LightType::Directional;
};