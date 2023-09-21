#include "lighting.h"
#include "../../xenity.h"

#if defined(EDITOR)
#include "../../xenity_editor.h"
#endif

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
	//SetReflection();
}

/*void Light::SetReflection()
{
	reflectedVariables["intensity"] = intensity;
	reflectedVariables["range"] = range;
	reflectedVariables["spotAngle"] = spotAngle;
	reflectedVariables["spotSmoothness"] = spotSmoothness;
	reflectedVariables["color"] = color;
}*/

Light::~Light()
{
	AssetManager::RemoveReflection(this);
}

std::unordered_map<std::string, ReflectionEntry> Light::GetReflection()
{
	std::unordered_map<std::string, ReflectionEntry> reflectedVariables;
	Reflection::AddReflectionVariable(reflectedVariables, intensity, "intensity", true);
	Reflection::AddReflectionVariable(reflectedVariables, range, "range", true);
	Reflection::AddReflectionVariable(reflectedVariables, spotAngle, "spotAngle", true);
	Reflection::AddReflectionVariable(reflectedVariables, spotSmoothness, "spotSmoothness", true);
	Reflection::AddReflectionVariable(reflectedVariables, color, "color", true);
	Reflection::AddReflectionVariable(reflectedVariables, (int&)type, "type", true);
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

void Light::SetupPointLight(const Color color, const float intensity, const float range)
{
	type = Light::Point;

	this->color = color;
	this->intensity = intensity;
	SetRange(range);
}

void Light::SetupDirectionalLight(const Color color, const float intensity)
{
	type = Light::Directional;

	this->color = color;
	this->intensity = intensity;
	quadratic = 0;
	linear = 0;
}

void Light::SetupSpotLight(const Color color, const float intensity, const float range, const float angle)
{
	SetupSpotLight(color, intensity, range, angle, spotSmoothness);
}

void Light::SetupSpotLight(const Color color, const float intensity, const float range, const float angle, const float smoothness)
{
	type = Light::Spot;

	this->color = color;
	this->intensity = intensity;
	SetRange(range);
	SetSpotAngle(angle);
	SetSpotSmoothness(smoothness);
}

#pragma endregion

#pragma region Accessors

void Light::OnDrawGizmos()
{
#if defined(EDITOR)
	auto transform = GetTransform();
	float distance = Vector3::Distance(transform->GetPosition(), Graphics::usedCamera.lock()->GetTransform()->GetPosition());
	float alpha = 1;
	if (distance <= 1.3f)
		alpha = distance - 0.3f;

	Color newColor = color;
	RGBA rgba = newColor.GetRGBA();
	newColor.SetFromRGBAfloat(rgba.r, rgba.g, rgba.b, alpha);
	SpriteManager::DrawSprite(transform->GetPosition(), Graphics::usedCamera.lock()->GetTransform()->GetRotation(), Vector3(0.2f), EditorUI::icons[Icon_Light], newColor);
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

void Light::SetRange(float value)
{
	if (value < 0)
		value = 0;

	range = value;
	if(type != Directional)
		UpdateLightValues();
}

#pragma endregion