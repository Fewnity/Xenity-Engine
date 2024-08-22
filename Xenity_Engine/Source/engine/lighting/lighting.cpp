// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "lighting.h"

#define _USE_MATH_DEFINES
#if defined(__PSP__) || defined(__vita__)
#undef __STRICT_ANSI__
#endif
#include <cmath>

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/game_elements/transform.h>
#include <engine/vectors/vector2.h>
#include <engine/graphics/graphics.h>
#include <engine/engine.h>

#pragma region Constructors / Destructor

Light::Light()
{
	m_componentName = "Light";
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
	Reflective::AddVariable(reflectedVariables, m_type, "type", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_intensity, "intensity", true);
	Reflective::AddVariable(reflectedVariables, m_range, "range", m_type != LightType::Directional && m_type != LightType::Ambient);
	Reflective::AddVariable(reflectedVariables, m_spotAngle, "spotAngle", m_type == LightType::Spot);
	Reflective::AddVariable(reflectedVariables, m_spotSmoothness, "spotSmoothness", m_type == LightType::Spot);
	return reflectedVariables;
}

void Light::OnReflectionUpdated()
{
	SetRange(m_range);
	SetSpotAngle(m_spotAngle);
	SetSpotSmoothness(m_spotSmoothness);
}

#pragma endregion

#pragma region Quick light setup

void Light::SetupPointLight(const Color &_color, const float _intensity, const float _range)
{
	this->m_type = LightType::Point;

	this->color = _color;
	SetIntensity(_intensity);
	SetRange(_range);
}

void Light::SetupDirectionalLight(const Color &_color, const float _intensity)
{
	this->m_type = LightType::Directional;

	this->color = _color;
	SetIntensity(_intensity);
	this->m_quadratic = 0;
	this->m_linear = 0;
}

void Light::SetupAmbientLight(const Color& _color, const float _intensity)
{
	this->m_type = LightType::Ambient;

	this->color = _color;
	SetIntensity(_intensity);
	this->m_quadratic = 0;
	this->m_linear = 0;
}

void Light::SetupSpotLight(const Color &_color, const float _intensity, const float _range, const float _angle)
{
	SetupSpotLight(_color, _intensity, _range, _angle, m_spotSmoothness);
}

void Light::SetupSpotLight(const Color &_color, const float _intensity, const float _range, const float _angle, const float _smoothness)
{
	this->m_type = LightType::Spot;

	this->color = _color;
	this->m_intensity = _intensity;
	SetRange(_range);
	SetSpotAngle(_angle);
	SetSpotSmoothness(_smoothness);
}

#pragma endregion

#pragma region Accessors

void Light::OnDrawGizmos()
{
#if defined(EDITOR)
	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	IconName icon = IconName::Icon_Point_Light;
	if(m_type == LightType::Directional || m_type == LightType::Ambient)
		icon = IconName::Icon_Sun_Light;
	else if (m_type == LightType::Spot)
		icon = IconName::Icon_Spot_Light;

	Gizmo::DrawBillboard(GetTransform()->GetPosition(), Vector2(0.2f), EditorUI::icons[(int)icon], color);

#endif
}

void Light::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	Gizmo::SetColor(Color::CreateFromRGBA(255, 245, 130, 255));

	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	if (m_type == LightType::Point) 
	{

		const float fixedLinear = (0.7f * 7.0f) / (m_range);
		const float fixedQuadratic = (7 * 1.8f) / ((powf(m_range, 2) / 6.0f));
		const float minIntensity = 0.05f;
		const float dis = (-minIntensity * fixedLinear + sqrtf(powf(minIntensity * fixedLinear, 2) - 4 * minIntensity * fixedQuadratic * (minIntensity - 1))) / (2 * minIntensity * fixedQuadratic);

		Gizmo::DrawSphere(GetTransform()->GetPosition(), dis);
	}
	else if (m_type == LightType::Directional || m_type == LightType::Spot)
	{
		Gizmo::DrawLine(GetTransform()->GetPosition(), GetTransform()->GetPosition() + GetTransform()->GetForward() * 3);
	}
#endif
}

void Light::UpdateLightValues()
{
	float tempInsity = m_intensity;
	if(m_intensity == 0)
		tempInsity = 0.0001f;

	m_linear = (0.7f * 7.0f) / (m_range / tempInsity);
	m_quadratic = (7 * 1.8f) / ((powf(m_range, 2) / 6.0f) / tempInsity);
}


void Light::SetSpotAngle(float angle)
{
	if (angle < 0)
		angle = 0;
	else if (angle > 90)
		angle = 90;

	m_spotAngle = angle;
}

void Light::SetSpotSmoothness(float smoothness)
{
	if (smoothness < 0)
		smoothness = 0;
	else if (smoothness > 1)
		smoothness = 1;

	m_spotSmoothness = smoothness;
}

void Light::SetIntensity(float intensity)
{
	if (intensity < 0)
		intensity = 0;
	this->m_intensity = intensity;
}

void Light::SetRange(float value)
{
	if (value < 0)
		value = 0;

	m_range = value;
	if (m_type != LightType::Directional)
		UpdateLightValues();
}

#pragma endregion