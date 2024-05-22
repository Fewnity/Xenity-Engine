// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector3.h>
#include <vector>
#include <random>

class MeshData;
class Material;

ENUM(EmitterShape, Box, Cone);

class Particle 
{
public:
	Vector3 position;
	Vector3 direction;
	float currentSpeed = 1;
	float currentLifeTime = 0;
	float lifeTime = 1;
	bool isDead = true;
};

/**
* @brief Component to spawn particles
*/
class API ParticleSystem : public IDrawable
{
public:
	ParticleSystem();
	~ParticleSystem();

	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;

	void Start() override;

	void Update() override;

	/**
	 * @brief [Internal]
	 */
	int GetDrawPriority() const override;

	/**
	* @brief Create the render commands
	*/
	void CreateRenderCommands(RenderBatch& renderBatch) override;

	/**
	* @brief Draw the command
	*/
	void DrawCommand(const RenderCommand& renderCommand) override;

	/**
	* Emitte particle when not in loop mode
	*/
	void Play();

	void SetIsEmitting(bool _isEmitting) 
	{
		isEmitting = _isEmitting;
	}

	bool GetIsEmitting() 
	{
		return isEmitting;
	}

private:
	bool isEmitting = true;

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

	std::shared_ptr <MeshData> mesh = nullptr;
	std::shared_ptr <Material> material = nullptr;
	std::shared_ptr<Texture> texture = nullptr;

	std::vector<Particle> particles;
	EmitterShape emitterShape = EmitterShape::Cone;
	float coneAngle = 20;
	bool reset = false;
	float lifeTimeMin = 5;
	float lifeTimeMax = 10;
	float speedMin = 1;
	float speedMax = 2;
	std::default_random_engine gen;
	std::uniform_real_distribution<float>speedDistribution;
	std::uniform_real_distribution<float>lifeTimeDistribution;
	std::uniform_real_distribution<float>boxXDistribution;
	std::uniform_real_distribution<float>boxYDistribution;
	std::uniform_real_distribution<float>boxZDistribution;

	bool isBillboard = true;
	float spawnRate = 1;
	float timer = 0;
	float maxParticles = 10;
	Color color = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	bool loop = true;
	bool play = false;

	Vector3 boxSize = Vector3(1);
	Vector3 direction = Vector3(0,1,0);

	void OnDrawGizmosSelected() override;

};