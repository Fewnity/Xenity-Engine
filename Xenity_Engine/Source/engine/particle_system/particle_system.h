// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <random>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector3.h>

class MeshData;
class Material;

ENUM(EmitterShape, Box, Cone);

/**
* @brief Component to spawn particles
*/
class API ParticleSystem : public IDrawable
{
public:
	ParticleSystem();
	~ParticleSystem();

	/**
	* Emitte particle when not in loop mode
	*/
	void Play();

	inline void SetIsEmitting(bool isEmitting)
	{
		m_isEmitting = isEmitting;
	}

	inline bool IsEmitting() const
	{
		return m_isEmitting;
	}

protected:
	struct Particle
	{
		Vector3 position;
		Vector3 direction;
		float currentSpeed = 1;
		float currentLifeTime = 0;
		float lifeTime = 1;
		bool isDead = true;
	};

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	void Start() override;

	void Update() override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	bool m_isEmitting = true;

	/**
	* @brief Called when the component is disabled
	*/
	void OnDisabled() override;

	/**
	* @brief Called when the component is enabled
	*/
	void OnEnabled() override;
	void ResetParticle(Particle& particle, bool setIsDead);
	void AllocateParticlesMemory();

	std::shared_ptr <MeshData> m_mesh = nullptr;
	std::shared_ptr <Material> m_material = nullptr;
	std::shared_ptr<Texture> m_texture = nullptr;

	std::vector<Particle> m_particles;
	EmitterShape m_emitterShape = EmitterShape::Cone;
	float m_coneAngle = 20;
	bool m_reset = false;
	float m_lifeTimeMin = 5;
	float m_lifeTimeMax = 10;
	float m_speedMin = 1;
	float m_speedMax = 2;
	static std::default_random_engine m_gen;
	std::uniform_real_distribution<float>m_speedDistribution;
	std::uniform_real_distribution<float>m_lifeTimeDistribution;
	std::uniform_real_distribution<float>m_boxXDistribution;
	std::uniform_real_distribution<float>m_boxYDistribution;
	std::uniform_real_distribution<float>m_boxZDistribution;

	bool m_isBillboard = true;
	float m_spawnRate = 1;
	float m_timer = 0;
	float m_maxParticles = 10;
	Color m_color = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	bool m_loop = true;
	bool m_play = false;

	Vector3 m_boxSize = Vector3(1);
	Vector3 m_direction = Vector3(0,1,0);

	void OnDrawGizmosSelected() override;

};