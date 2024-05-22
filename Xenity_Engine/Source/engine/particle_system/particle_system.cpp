// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "particle_system.h"
#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/3d_graphics/mesh_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/material.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/tools/math.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/time/time.h>
#include <random>
#if defined(EDITOR)
#include <editor/gizmo.h>
#endif
#include <engine/engine.h>
#include <glm/ext/matrix_transform.hpp>
#include <engine/graphics/2d_graphics/sprite_manager.h>

ParticleSystem::ParticleSystem()
{
	componentName = "ParticleSystem";
	AssetManager::AddReflection(this);

	AllocateParticlesMemory();
}

ParticleSystem::~ParticleSystem()
{
	AssetManager::RemoveReflection(this);
}

ReflectiveData ParticleSystem::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, mesh, "mesh", true);
	Reflective::AddVariable(reflectedVariables, material, "material", true);
	Reflective::AddVariable(reflectedVariables, texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, color, "color", true);
	Reflective::AddVariable(reflectedVariables, isBillboard, "isBillboard", true);

	Reflective::AddVariable(reflectedVariables, emitterShape, "emitterShape", true);
	Reflective::AddVariable(reflectedVariables, coneAngle, "coneAngle", emitterShape == EmitterShape::Cone);
	Reflective::AddVariable(reflectedVariables, boxSize, "boxSize", emitterShape == EmitterShape::Box);
	Reflective::AddVariable(reflectedVariables, direction, "direction", emitterShape == EmitterShape::Box);

	Reflective::AddVariable(reflectedVariables, speedMin, "speedMin", true);
	Reflective::AddVariable(reflectedVariables, speedMax, "speedMax", true);
	Reflective::AddVariable(reflectedVariables, lifeTimeMin, "lifeTimeMin", true);
	Reflective::AddVariable(reflectedVariables, lifeTimeMax, "lifeTimeMax", true);

	Reflective::AddVariable(reflectedVariables, spawnRate, "spawnRate", true);
	Reflective::AddVariable(reflectedVariables, maxParticles, "maxParticles", true);

	//Reflective::AddVariable(reflectedVariables, reset, "reset", true);
	Reflective::AddVariable(reflectedVariables, isEmitting, "isEmitting", true);
	Reflective::AddVariable(reflectedVariables, loop, "loop", true);
	Reflective::AddVariable(reflectedVariables, play, "play", !loop);

	return reflectedVariables;
}

void ParticleSystem::OnReflectionUpdated()
{
	Graphics::isRenderingBatchDirty = true;
	if (speedMin > speedMax)
		speedMin = speedMax;
	else if (speedMax < speedMin)
		speedMax = speedMin;

	if (lifeTimeMin > lifeTimeMax)
		lifeTimeMin = lifeTimeMax;
	else if (lifeTimeMax < lifeTimeMin)
		lifeTimeMax = lifeTimeMin;

	speedDistribution = std::uniform_real_distribution<float>(speedMin, speedMax);
	lifeTimeDistribution = std::uniform_real_distribution<float>(lifeTimeMin, lifeTimeMax);

	boxSize.x = abs(boxSize.x);
	boxSize.y = abs(boxSize.y);
	boxSize.z = abs(boxSize.z);

	boxXDistribution = std::uniform_real_distribution<float>(-boxSize.x / 2.0f, boxSize.x / 2.0f);
	boxYDistribution = std::uniform_real_distribution<float>(-boxSize.y / 2.0f, boxSize.y / 2.0f);
	boxZDistribution = std::uniform_real_distribution<float>(-boxSize.z / 2.0f, boxSize.z / 2.0f);

	AllocateParticlesMemory();
}

void ParticleSystem::Start()
{
	for (int i = 0; i < maxParticles; i++)
	{
		ResetParticle(particles[i], true);
	}
}

void ParticleSystem::Update()
{

}

void ParticleSystem::Play()
{
	for (int i = 0; i < maxParticles; i++)
	{
		ResetParticle(particles[i], false);
	}
}

void ParticleSystem::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	const Color lineColor = Color::CreateFromRGBAFloat(0, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	Vector3 pos = GetTransform()->GetPosition();

	if (emitterShape == EmitterShape::Box)
	{
		// Bottom vertex
		const Vector3 v1 = pos + Vector3(-boxSize.x, -boxSize.y, -boxSize.z) / 2.0f;
		const Vector3 v2 = pos + Vector3(-boxSize.x, -boxSize.y, boxSize.z) / 2.0f;
		const Vector3 v3 = pos + Vector3(boxSize.x, -boxSize.y, -boxSize.z) / 2.0f;
		const Vector3 v4 = pos + Vector3(boxSize.x, -boxSize.y, boxSize.z) / 2.0f;

		// Top vertex
		const Vector3 v5 = pos + Vector3(-boxSize.x, boxSize.y, -boxSize.z) / 2.0f;
		const Vector3 v6 = pos + Vector3(-boxSize.x, boxSize.y, boxSize.z) / 2.0f;
		const Vector3 v7 = pos + Vector3(boxSize.x, boxSize.y, -boxSize.z) / 2.0f;
		const Vector3 v8 = pos + Vector3(boxSize.x, boxSize.y, boxSize.z) / 2.0f;

		Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

		// Bottom
		Gizmo::DrawLine(v1, v2);
		Gizmo::DrawLine(v1, v3);
		Gizmo::DrawLine(v4, v3);
		Gizmo::DrawLine(v4, v2);

		// Top
		Gizmo::DrawLine(v5, v6);
		Gizmo::DrawLine(v5, v7);
		Gizmo::DrawLine(v8, v7);
		Gizmo::DrawLine(v8, v6);

		// Bottom to top
		Gizmo::DrawLine(v1, v5);
		Gizmo::DrawLine(v2, v6);
		Gizmo::DrawLine(v3, v7);
		Gizmo::DrawLine(v4, v8);
	}
#endif
}

void ParticleSystem::ResetParticle(Particle& particle, bool setIsDead)
{
	if (emitterShape == EmitterShape::Cone)
	{
		particle.position = Vector3(0);
		particle.direction = Vector3((rand() % 2000 - 1000) / 1000.0f * coneAngle / 180.0f, (rand() % 1000) / 1000.0f + (180 - coneAngle) / 180.0f, (rand() % 2000 - 1000) / 1000.0f * coneAngle / 180.0f);
	}
	else if (emitterShape == EmitterShape::Box)
	{
		particle.position = Vector3(boxXDistribution(gen), boxYDistribution(gen), boxZDistribution(gen));
		particle.direction = direction;
	}

	particle.direction.Normalize();

	particle.currentSpeed = speedDistribution(gen);

	particle.currentLifeTime = 0;

	particle.lifeTime = lifeTimeDistribution(gen);
	particle.isDead = setIsDead;
}

void ParticleSystem::AllocateParticlesMemory()
{
	particles.clear();
	for (int i = 0; i < maxParticles; i++)
	{
		Particle newParticle = Particle();
		ResetParticle(newParticle, true);
		particles.push_back(newParticle);
	}
}

void ParticleSystem::DrawCommand(const RenderCommand& renderCommand)
{
	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = renderCommand.material->useLighting;
	renderSettings.useBlend = renderCommand.material->useTransparency;

	Vector3 rotation = Vector3(0);

	const int camCount = Graphics::cameras.size();

	const Vector3& camScale = Graphics::usedCamera->GetTransform()->GetScale();
	const glm::mat4& camMat = Graphics::usedCamera->GetTransform()->GetTransformationMatrix();
	const glm::mat4& transMat = GetTransform()->GetTransformationMatrix();

	const RGBA& rgba = color.GetRGBA();
	const Vector3& scale = GetTransform()->GetScale();
	const glm::vec3 fixedScale = glm::vec3(1.0f / camScale.x, 1.0f / camScale.z, 1.0f / camScale.y) * glm::vec3(scale.x, scale.y, scale.z);

	for (int i = 0; i < maxParticles; i++)
	{
		Particle& particle = particles[i];
		if (particle.isDead)
		{
			continue;
		}

		glm::mat4 newMat = Math::MultiplyMatrices(transMat, Math::CreateModelMatrix(particle.position, rotation, Vector3(1)));
		if (isBillboard)
		{
			for (int matI = 0; matI < 3; matI++)
			{
				for (int matJ = 0; matJ < 3; matJ++)
				{
					newMat[matI][matJ] = camMat[matI][matJ];
				}
			}
			newMat = glm::scale(newMat, fixedScale); // Fix scale if the camera has a scale (Y and Z are inverted for some raison)
		}

		renderCommand.subMesh->meshData->unifiedColor.SetFromRGBAfloat(rgba.r, rgba.g, rgba.b, sin((particle.currentLifeTime / particle.lifeTime) * Math::PI));

		Graphics::DrawSubMesh(*renderCommand.subMesh, material, texture, renderSettings, newMat, false);

		particle.position += particle.direction * Time::GetDeltaTime() / camCount * particle.currentSpeed;

		particle.currentLifeTime += Time::GetDeltaTime() / camCount;
		if (particle.currentLifeTime >= particle.lifeTime)
		{
			particle.isDead = true;
		}
	}

	// Spawn new particles
	if (isEmitting && loop)
	{
		timer += Time::GetDeltaTime() / camCount * spawnRate;
		while (timer > 1)
		{
			timer -= 1;
			bool found = false;
			for (int i = 0; i < maxParticles; i++)
			{
				Particle& particle = particles[i];
				if (particle.isDead)
				{
					ResetParticle(particle, false);
					found = true;
					break;
				}
			}

			if (!found)
			{
				timer = 0;
			}
		}
	}

	/*if (reset)
	{
		reset = false;
		for (int i = 0; i < maxParticles; i++)
		{
			ResetParticle(particles[i], false);
		}
	}*/

	if (play)
	{
		play = false;
		Play();
	}
}

void ParticleSystem::OnDisabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void ParticleSystem::OnEnabled()
{
	Graphics::isRenderingBatchDirty = true;
}

void ParticleSystem::CreateRenderCommands(RenderBatch& renderBatch)
{
	/*if (!mesh)
		return;*/

	if (material == nullptr || texture == nullptr)
		return;

	RenderCommand command = RenderCommand();
	command.material = material;
	command.drawable = this;
	if (!mesh)
		command.subMesh = SpriteManager::spriteMeshData->subMeshes[0];
	else
		command.subMesh = mesh->subMeshes[0];
	command.transform = GetTransform();
	command.isEnabled = GetIsEnabled() && GetGameObject()->GetLocalActive();
	if (!material->useTransparency)
	{
		RenderQueue& renderQueue = renderBatch.renderQueues[material->fileId];
		renderQueue.commands.push_back(command);
		renderQueue.commandIndex++;
	}
	else
	{
		renderBatch.transparentMeshCommands.push_back(command);
		renderBatch.transparentMeshCommandIndex++;
	}
}

int ParticleSystem::GetDrawPriority() const
{
	return 0;
}
