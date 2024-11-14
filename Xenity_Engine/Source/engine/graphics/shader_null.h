// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/graphics/shader.h>

/**
* @brief [Internal] Shader file class
*/
class ShaderNull : public Shader
{
public:
	void Load() override {}
	void CreateShader(Shader::ShaderType type) override {}

	bool Use() override { return false; }

	void SetShaderCameraPosition() override {}

	void SetShaderCameraPositionCanvas() override {}

	void SetShaderProjection() override {}

	void SetShaderProjectionCanvas() override {}

	void SetShaderModel(const glm::mat4& trans) override {}

	void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale) override {}

	void SetLightIndices(const LightsIndices& lightsIndices) override {}

	void SetShaderAttribut(const std::string& attribut, const Vector4& value) override {}
	void SetShaderAttribut(const std::string& attribut, const Vector3& value) override {}
	void SetShaderAttribut(const std::string& attribut, const Vector2& value) override {}
	void SetShaderAttribut(const std::string& attribut, float value) override {}
	void SetShaderAttribut(const std::string& attribut, int value) override {}

	void UpdateLights(bool disableLights) override {}

	void Link() override {}

	bool Compile(const std::string& filePath, ShaderType type) override { return false; }

	void SetPointLightData(const Light& light, const int index) override {}
	void SetDirectionalLightData(const Light& light, const int index) override {}
	void SetAmbientLightData(const Vector3& color) override {}
	void SetSpotLightData(const Light& light, const int index) override {}
};