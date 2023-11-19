#include "lighting.h"
#include "../../xenity.h"
#include "../../xenity_editor.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#pragma region Constructors / Destructor

Light::Light()
{
	componentName = "Light";
	AssetManager::AddReflection(this);
}

Light::~Light()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData Light::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, intensity, "intensity", true);
	Reflective::AddVariable(reflectedVariables, range, "range", true);
	Reflective::AddVariable(reflectedVariables, spotAngle, "spotAngle", true);
	Reflective::AddVariable(reflectedVariables, spotSmoothness, "spotSmoothness", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, (int&)type, "type", true);
	return reflectedVariables;
}

void Light::OnReflectionUpdated()
{
	SetRange(range);
	SetSpotAngle(spotAngle);
	SetSpotSmoothness(spotSmoothness);
}

#pragma endregion

#pragma region Quick light setup

void Light::SetupPointLight(const Color& _color, const float _intensity, const float _range)
{
	this->type = Light::Point;

	this->color = _color;
	this->intensity = _intensity;
	SetRange(_range);
}

void Light::SetupDirectionalLight(const Color& _color, const float _intensity)
{
	this->type = Light::Directional;

	this->color = _color;
	this->intensity = _intensity;
	this->quadratic = 0;
	this->linear = 0;
}

void Light::SetupSpotLight(const Color& _color, const float _intensity, const float _range, const float _angle)
{
	SetupSpotLight(_color, _intensity, _range, _angle, spotSmoothness);
}

void Light::SetupSpotLight(const Color& _color, const float _intensity, const float _range, const float _angle, const float _smoothness)
{
	this->type = Light::Spot;

	this->color = _color;
	this->intensity = _intensity;
	SetRange(_range);
	SetSpotAngle(_angle);
	SetSpotSmoothness(_smoothness);
}

#pragma endregion

#pragma region Accessors

void Light::OnDrawGizmos()
{
#if defined(EDITOR)
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[Icon_Light], color);
#endif
}

void Light::UpdateLightValues()
{
	linear = (0.7f * 7.0f) / range;
	quadratic = (7 * 1.8f) / (powf(range, 2) / 6.0f);
}

float Light::GetRange() const
{
	return range;
}

void Light::SetSpotAngle(float angle)
{
	if (angle < 0)
		angle = 0;
	else if (angle > 179)
		angle = 179;

	spotAngle = angle;
}

void Light::SetSpotSmoothness(float smoothness)
{
	if (smoothness < 0)
		smoothness = 0;
	else if (smoothness > 1)
		smoothness = 1;

	spotSmoothness = smoothness;
}

float Light::GetSpotAngle() const
{
	return spotAngle;
}

float Light::GetSpotSmoothness() const
{
	return spotSmoothness;
}

void Light::SetIntensity(float intensity)
{
	this->intensity = intensity;
}

float Light::GetIntensity() const
{
	return intensity;
}

void Light::SetRange(float value)
{
	if (value < 0)
		value = 0;

	range = value;
	if (type != Directional)
		UpdateLightValues();
}

#pragma endregion