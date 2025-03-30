// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <random>

#include <glm/ext/matrix_transform.hpp>

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/quaternion.h>

class MeshData;
class Material;

ENUM(EmitterShape, Box, Cone);
ENUM(SimulationRate, fps_30, fps_60, every_frame);

/**
* @brief Component to spawn particles
*/
class API ParticleSystem : public IDrawable
{
public:
	ParticleSystem();
	~ParticleSystem();

	/**
	* Emitte all particles when not in loop mode
	*/
	void Play();

	/**
	* Remove all particles and restart the emission
	*/
	void ResetParticles();

	/**
	* @brief Set if the particle system is emitting particles
	*/
	inline void SetIsEmitting(bool isEmitting)
	{
		m_isEmitting = isEmitting;
	}

	/**
	* @brief Get if the particle system is emitting particles
	*/
	inline bool IsEmitting() const
	{
		return m_isEmitting;
	}

	/**
	* @brief Set you own function to set the global scale of the particle over its life time
	* @brief The function takes the life time ratio [0;1] and return the scale of the particle
	* @param function The function to set, should be static (nullptr to use the default function)
	*/
	void SetScaleOverLifeTimeFunction(float (*function)(float))
	{
		if (function == nullptr)
		{
			scaleOverLifeTimeFunction = DefaultGetScaleOverLifeTime;
		}
		else
		{
			scaleOverLifeTimeFunction = function;
		}
	}

	/**
	* @brief Set you own function to set the speed multiplier of the particle over its life time
	* @brief The function takes the life time ratio [0;1] and return the speed of the particle
	* @param function The function to set, should be static (nullptr to use the default function)
	*/
	void SetSpeedMultiplierOverLifeTimeFunction(float (*function)(float))
	{
		if (function == nullptr)
		{
			speedMultiplierOverLifeTimeFunction = DefaultGetSpeedOverLifeTime;
		}
		else
		{
			speedMultiplierOverLifeTimeFunction = function;
		}
	}

	/**
	* @brief Set you own function to set the color of the particle over its life time
	* @brief The function takes the life time ratio [0;1] and return the color of the particle
	* @brief x = red, y = green, z = blue, w = alpha (0 to 1)
	* @param function The function to set, should be static (nullptr to use the default function)
	*/
	void SetColorOverLifeTimeFunction(Vector4 (*function)(float))
	{
		if (function == nullptr)
		{
			colorOverLifeTimeFunction = DefaultGetColorOverLifeTime;
		}
		else 
		{
			colorOverLifeTimeFunction = function;
		}
	}

protected:
	struct Particle
	{
		glm::mat4 matrix;
		Vector3 position;
		Vector3 direction;
		Quaternion rotation;
		float billboardRotation = 0;
		float speedMultiplier = 1;
		float currentSpeed = 1;
		float currentLifeTime = 0;
		float scale = 1;
		float lifeTime = 1;
		float simulationCooldown = 0;
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

	void OnNewRender(int cameraIndex) override;

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

	static float DefaultGetScaleOverLifeTime(float lifeTime);
	static float DefaultGetSpeedOverLifeTime(float lifeTime);
	static Vector4 DefaultGetColorOverLifeTime(float lifeTime);

	std::shared_ptr<MeshData> m_mesh = nullptr;
	std::shared_ptr<Material> m_material = nullptr;
	std::shared_ptr<Texture> m_texture = nullptr;

	std::vector<Particle> m_particles;
	EmitterShape m_emitterShape = EmitterShape::Cone;
	float m_coneAngle = 20;
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

	float (*scaleOverLifeTimeFunction)(float) = nullptr;
	float (*speedMultiplierOverLifeTimeFunction)(float) = nullptr;
	Vector4(*colorOverLifeTimeFunction)(float) = nullptr;

	Vector3 m_boxSize = Vector3(1);
	Vector3 m_direction = Vector3(0,1,0);
	Color m_color = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	float m_spawnRate = 1;
	float m_timer = 0;
	float m_maxParticles = 10;
	SimulationRate m_simulationRate = SimulationRate::fps_60;
	bool m_randomRotation = true;
	bool m_isBillboard = true;
	bool m_isEmitting = true;
	bool m_loop = true;
	bool m_play = false;
	bool m_worldSimulation = true;
	bool m_reset = false;

	void OnDrawGizmosSelected() override;
};