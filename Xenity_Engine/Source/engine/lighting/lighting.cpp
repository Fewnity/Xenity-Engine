#include "lighting.h"

#include <engine/asset_management/asset_manager.h>

#include <engine/game_elements/transform.h>
#include <engine/vectors/vector2.h>
#include <engine/tools/math.h>

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#endif

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>
#include <engine/graphics/graphics.h>

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

void Light::RemoveReferences()
{
	AssetManager::RemoveLight(std::dynamic_pointer_cast<Light>(shared_from_this()));
}

ReflectiveData Light::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, type, "type", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, intensity, "intensity", true);
	Reflective::AddVariable(reflectedVariables, range, "range", type != LightType::Directional);
	Reflective::AddVariable(reflectedVariables, spotAngle, "spotAngle", type == LightType::Spot);
	Reflective::AddVariable(reflectedVariables, spotSmoothness, "spotSmoothness", type == LightType::Spot);
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

void Light::SetupPointLight(const Color &_color, const float _intensity, const float _range)
{
	this->type = LightType::Point;

	this->color = _color;
	SetIntensity(_intensity);
	SetRange(_range);
}

void Light::SetupDirectionalLight(const Color &_color, const float _intensity)
{
	this->type = LightType::Directional;

	this->color = _color;
	SetIntensity(_intensity);
	this->quadratic = 0;
	this->linear = 0;
}

void Light::SetupSpotLight(const Color &_color, const float _intensity, const float _range, const float _angle)
{
	SetupSpotLight(_color, _intensity, _range, _angle, spotSmoothness);
}

void Light::SetupSpotLight(const Color &_color, const float _intensity, const float _range, const float _angle, const float _smoothness)
{
	this->type = LightType::Spot;

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
	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[(int)IconName::Icon_Light], color);

	//Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	//Gizmo::SetColor(lineColor);

	//Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	//const int steps = 30;
	//const float angleStep = 360.0f / steps;
	//for (int i = 0; i < steps; i++)
	//{
	//	// Draw sphere with lines
	//	{
	//		Vector3 pos0 = GetTransform()->GetPosition();
	//		pos0.x += 5 * cos((angleStep * i) * Math::PI / 180.0f);
	//		pos0.z += 5 * sin((angleStep * i) * Math::PI / 180.0f);
	//		//pos0.x = -pos0.x;

	//		Vector3 pos1 = GetTransform()->GetPosition();
	//		pos1.x += 5 * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		pos1.z += 5 * sin((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		//pos1.x = -pos1.x;

	//		Gizmo::DrawLine(pos0, pos1);
	//	}

	//	{
	//		Vector3 pos0 = GetTransform()->GetPosition();
	//		pos0.x += 5 * cos((angleStep * i) * Math::PI / 180.0f);
	//		pos0.y += 5 * sin((angleStep * i) * Math::PI / 180.0f);
	//		//pos0.x = -pos0.x;

	//		Vector3 pos1 = GetTransform()->GetPosition();
	//		pos1.x += 5 * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		pos1.y += 5 * sin((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		//pos1.x = -pos1.x;

	//		Gizmo::DrawLine(pos0, pos1);
	//	}

	//	{
	//		Vector3 pos0 = GetTransform()->GetPosition();
	//		pos0.y += 5 * cos((angleStep * i) * Math::PI / 180.0f);
	//		pos0.z += 5 * sin((angleStep * i) * Math::PI / 180.0f);
	//		//pos0.x = -pos0.x;

	//		Vector3 pos1 = GetTransform()->GetPosition();
	//		pos1.y += 5 * cos((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		pos1.z += 5 * sin((angleStep * (i + 1)) * Math::PI / 180.0f);
	//		//pos1.x = -pos1.x;

	//		Gizmo::DrawLine(pos0, pos1);
	//	}
	//}
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
	if (intensity < 0)
		intensity = 0;
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
	if (type != LightType::Directional)
		UpdateLightValues();
}

#pragma endregion