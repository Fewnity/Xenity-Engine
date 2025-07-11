// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/graphics/shader/shader.h>

/**
* @brief [Internal] Shader file class
*/
class ShaderNull : public Shader
{
public:
	void Load(const LoadOptions& loadOptions) override {}
	void CreateShader(Shader::ShaderType type) override {}

	bool Use() override { return false; }

	void SetShaderCameraPosition() override {}

	void SetShaderCameraPositionCanvas() override {}

	void SetShaderProjection() override {}

	void SetShaderProjectionCanvas() override {}

	void SetShaderModel(const glm::mat4& trans, const glm::mat3& normalMatrix, const glm::mat4& mvpMatrix) override {}

	void SetShaderModel(const Vector3& position, const Vector3& eulerAngle, const Vector3& scale) override {}

	void SetLightIndices(const LightsIndices& lightsIndices) override {}

	void SetShaderAttribut(const std::string& attribut, const Vector4& value) override {}
	void SetShaderAttribut(const std::string& attribut, const Vector3& value) override {}
	void SetShaderAttribut(const std::string& attribut, const Vector2& value) override {}
	void SetShaderAttribut(const std::string& attribut, float value) override {}
	void SetShaderAttribut(const std::string& attribut, int value) override {}

	void UpdateLights() override {}

	void Link() override {}

	[[nodiscard]] bool Compile(const std::string& filePath, ShaderType type) override { return false; }

	void SetPointLightData(const Light& light, const int index) override {}
	void SetDirectionalLightData(const Light& light, const int index) override {}
	void SetAmbientLightData(const Vector3& color) override {}
	void SetSpotLightData(const Light& light, const int index) override {}

	void SetShaderOffsetAndTiling(const Vector2& offset, const Vector2& tiling) override {}
	void SetAlphaThreshold(float alphaThreshold) override {}
};