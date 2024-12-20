// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "particle_system.h"

#include <random>
#include <glm/ext/matrix_transform.hpp>

#if defined(EDITOR)
#include <editor/gizmo.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/tools/math.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/time/time.h>
#include <engine/vectors/quaternion.h>
#include <engine/engine.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/debug/performance.h>

ParticleSystem::ParticleSystem()
{
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
	Reflective::AddVariable(reflectedVariables, m_mesh, "mesh", true);
	Reflective::AddVariable(reflectedVariables, m_material, "material", true);
	Reflective::AddVariable(reflectedVariables, m_texture, "texture", true);
	Reflective::AddVariable(reflectedVariables, m_color, "color", true);
	Reflective::AddVariable(reflectedVariables, m_isBillboard, "isBillboard", true);

	Reflective::AddVariable(reflectedVariables, m_emitterShape, "emitterShape", true);
	Reflective::AddVariable(reflectedVariables, m_coneAngle, "coneAngle", m_emitterShape == EmitterShape::Cone);
	Reflective::AddVariable(reflectedVariables, m_boxSize, "boxSize", m_emitterShape == EmitterShape::Box);
	Reflective::AddVariable(reflectedVariables, m_direction, "direction", m_emitterShape == EmitterShape::Box);

	Reflective::AddVariable(reflectedVariables, m_speedMin, "speedMin", true);
	Reflective::AddVariable(reflectedVariables, m_speedMax, "speedMax", true);
	Reflective::AddVariable(reflectedVariables, m_lifeTimeMin, "lifeTimeMin", true);
	Reflective::AddVariable(reflectedVariables, m_lifeTimeMax, "lifeTimeMax", true);

	Reflective::AddVariable(reflectedVariables, m_spawnRate, "spawnRate", true);
	Reflective::AddVariable(reflectedVariables, m_maxParticles, "maxParticles", true);

	//Reflective::AddVariable(reflectedVariables, reset, "reset", true);
	Reflective::AddVariable(reflectedVariables, m_isEmitting, "isEmitting", true);
	Reflective::AddVariable(reflectedVariables, m_loop, "loop", true);
	Reflective::AddVariable(reflectedVariables, m_play, "play", !m_loop);

	return reflectedVariables;
}

void ParticleSystem::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;
	if (m_speedMin > m_speedMax)
		m_speedMin = m_speedMax;
	else if (m_speedMax < m_speedMin)
		m_speedMax = m_speedMin;

	if (m_lifeTimeMin > m_lifeTimeMax)
		m_lifeTimeMin = m_lifeTimeMax;
	else if (m_lifeTimeMax < m_lifeTimeMin)
		m_lifeTimeMax = m_lifeTimeMin;

	m_speedDistribution = std::uniform_real_distribution<float>(m_speedMin, m_speedMax);
	m_lifeTimeDistribution = std::uniform_real_distribution<float>(m_lifeTimeMin, m_lifeTimeMax);

	m_boxSize.x = fabs(m_boxSize.x);
	m_boxSize.y = fabs(m_boxSize.y);
	m_boxSize.z = fabs(m_boxSize.z);

	m_boxXDistribution = std::uniform_real_distribution<float>(-m_boxSize.x / 2.0f, m_boxSize.x / 2.0f);
	m_boxYDistribution = std::uniform_real_distribution<float>(-m_boxSize.y / 2.0f, m_boxSize.y / 2.0f);
	m_boxZDistribution = std::uniform_real_distribution<float>(-m_boxSize.z / 2.0f, m_boxSize.z / 2.0f);

	AllocateParticlesMemory();
}

void ParticleSystem::Start()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		ResetParticle(m_particles[i], true);
	}
}

void ParticleSystem::Update()
{

}

void ParticleSystem::Play()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		ResetParticle(m_particles[i], false);
	}
}

void ParticleSystem::OnDrawGizmosSelected()
{
#if defined(EDITOR)
	const Color lineColor = Color::CreateFromRGBAFloat(0, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	const Vector3& pos = GetTransformRaw()->GetPosition();

	if (m_emitterShape == EmitterShape::Box)
	{
		// Bottom vertex
		const Vector3 v1 = pos + Vector3(-m_boxSize.x, -m_boxSize.y, -m_boxSize.z) / 2.0f;
		const Vector3 v2 = pos + Vector3(-m_boxSize.x, -m_boxSize.y, m_boxSize.z) / 2.0f;
		const Vector3 v3 = pos + Vector3(m_boxSize.x, -m_boxSize.y, -m_boxSize.z) / 2.0f;
		const Vector3 v4 = pos + Vector3(m_boxSize.x, -m_boxSize.y, m_boxSize.z) / 2.0f;

		// Top vertex
		const Vector3 v5 = pos + Vector3(-m_boxSize.x, m_boxSize.y, -m_boxSize.z) / 2.0f;
		const Vector3 v6 = pos + Vector3(-m_boxSize.x, m_boxSize.y, m_boxSize.z) / 2.0f;
		const Vector3 v7 = pos + Vector3(m_boxSize.x, m_boxSize.y, -m_boxSize.z) / 2.0f;
		const Vector3 v8 = pos + Vector3(m_boxSize.x, m_boxSize.y, m_boxSize.z) / 2.0f;

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
	if (m_emitterShape == EmitterShape::Cone)
	{
		particle.position = Vector3(0);
		particle.direction = Vector3((rand() % 2000 - 1000) / 1000.0f * m_coneAngle / 180.0f, (rand() % 1000) / 1000.0f + (180 - m_coneAngle) / 180.0f, (rand() % 2000 - 1000) / 1000.0f * m_coneAngle / 180.0f);
	}
	else if (m_emitterShape == EmitterShape::Box)
	{
		particle.position = Vector3(m_boxXDistribution(m_gen), m_boxYDistribution(m_gen), m_boxZDistribution(m_gen));
		particle.direction = m_direction;
	}

	particle.direction.Normalize();

	particle.currentSpeed = m_speedDistribution(m_gen);

	particle.currentLifeTime = 0;

	particle.lifeTime = m_lifeTimeDistribution(m_gen);
	particle.isDead = setIsDead;
}

void ParticleSystem::AllocateParticlesMemory()
{
	m_particles.clear();
	for (int i = 0; i < m_maxParticles; i++)
	{
		Particle newParticle = Particle();
		ResetParticle(newParticle, true);
		m_particles.push_back(newParticle);
	}
}

void ParticleSystem::DrawCommand(const RenderCommand& renderCommand)
{
	SCOPED_PROFILER("ParticleSystem::DrawCommand", scopeBenchmark);

	RenderingSettings renderSettings = RenderingSettings();
	renderSettings.invertFaces = false;
	renderSettings.useDepth = true;
	renderSettings.useTexture = true;
	renderSettings.useLighting = renderCommand.material->GetUseLighting();
	renderSettings.renderingMode = renderCommand.material->GetRenderingMode();

	static const Quaternion rotation = Quaternion::Identity();

	const size_t camCount = Graphics::cameras.size();

	const Vector3& camScale = Graphics::usedCamera->GetTransformRaw()->GetScale();
	const glm::mat4& camMat = Graphics::usedCamera->GetTransformRaw()->GetTransformationMatrix();
	const glm::mat4& transMat = GetTransformRaw()->GetTransformationMatrix();

	const RGBA& rgba = m_color.GetRGBA();
	const Vector3& scale = GetTransformRaw()->GetScale();
	const glm::vec3 fixedScale = glm::vec3(1.0f / camScale.x, 1.0f / camScale.z, 1.0f / camScale.y) * glm::vec3(scale.x, scale.y, scale.z);

	for (int i = 0; i < m_maxParticles; i++)
	{
		Particle& particle = m_particles[i];
		if (particle.isDead)
		{
			continue;
		}

		glm::mat4 newMat = Math::MultiplyMatrices(transMat, Math::CreateModelMatrix(particle.position, rotation, Vector3(1)));
		if (m_isBillboard)
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

		Graphics::DrawSubMesh(*renderCommand.subMesh, *m_material, m_texture.get(), renderSettings, newMat, false);

		particle.position += particle.direction * Time::GetDeltaTime() / static_cast<float>(camCount) * particle.currentSpeed;

		particle.currentLifeTime += Time::GetDeltaTime() / camCount;
		if (particle.currentLifeTime >= particle.lifeTime)
		{
			particle.isDead = true;
		}
	}

	// Spawn new particles
	if (m_isEmitting && m_loop)
	{
		m_timer += Time::GetDeltaTime() / camCount * m_spawnRate;
		while (m_timer > 1)
		{
			m_timer -= 1;
			bool found = false;
			for (int i = 0; i < m_maxParticles; i++)
			{
				Particle& particle = m_particles[i];
				if (particle.isDead)
				{
					ResetParticle(particle, false);
					found = true;
					break;
				}
			}

			if (!found)
			{
				m_timer = 0;
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

	if (m_play)
	{
		m_play = false;
		Play();
	}
}

void ParticleSystem::OnDisabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void ParticleSystem::OnEnabled()
{
	Graphics::s_isRenderingBatchDirty = true;
}

void ParticleSystem::CreateRenderCommands(RenderBatch& renderBatch)
{
	/*if (!mesh)
		return;*/

	if (m_material == nullptr || m_texture == nullptr)
		return;

	RenderCommand command = RenderCommand();
	command.material = m_material.get();
	command.drawable = this;
	if (!m_mesh)
		command.subMesh = SpriteManager::GetBasicSpriteMeshData()->m_subMeshes[0].get();
	else
		command.subMesh = m_mesh->m_subMeshes[0].get();
	command.transform = GetTransform().get();
	command.isEnabled = IsEnabled() && GetGameObjectRaw()->IsLocalActive();
	if (m_material->GetRenderingMode() == MaterialRenderingModes::Opaque || m_material->GetRenderingMode() == MaterialRenderingModes::Cutout)
	{
		RenderQueue& renderQueue = renderBatch.renderQueues[m_material->GetFileId()];
		renderQueue.commands.push_back(command);
		renderQueue.commandIndex++;
	}
	else
	{
		renderBatch.transparentMeshCommands.push_back(command);
		renderBatch.transparentMeshCommandIndex++;
	}
}
