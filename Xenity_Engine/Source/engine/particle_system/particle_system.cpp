// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "particle_system.h"

#include <random>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#if defined(EDITOR)
#include <editor/rendering/gizmo.h>
#include <engine/game_elements/gameplay_manager.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/graphics/renderer/renderer.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture/texture.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/tools/math.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/time/time.h>
#include <engine/math/quaternion.h>
#include <engine/engine.h>
#include <engine/graphics/2d_graphics/sprite_manager.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/debug/performance.h>
#include <engine/debug/debug.h>

std::default_random_engine ParticleSystem::m_gen;

ParticleSystem::ParticleSystem()
{
	AssetManager::AddReflection(this);
	scaleOverLifeTimeFunction = DefaultGetScaleOverLifeTime;
	speedMultiplierOverLifeTimeFunction = DefaultGetSpeedOverLifeTime;
	colorOverLifeTimeFunction = DefaultGetColorOverLifeTime;
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
	Reflective::AddVariable(reflectedVariables, m_randomRotation, "randomRotation", true);

	//Reflective::AddVariable(reflectedVariables, reset, "reset", true);
	Reflective::AddVariable(reflectedVariables, m_isEmitting, "isEmitting", true);
	Reflective::AddVariable(reflectedVariables, m_simulationRate, "simulationRate", true);
	Reflective::AddVariable(reflectedVariables, m_worldSimulation, "worldSimulation", true);
	Reflective::AddVariable(reflectedVariables, m_loop, "loop", true);
	Reflective::AddVariable(reflectedVariables, m_play, "play", !m_loop);

	return reflectedVariables;
}

void ParticleSystem::OnReflectionUpdated()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Graphics::s_isRenderingBatchDirty = true;

	// Check variables values and correct them if needed
	if (m_speedMin > m_speedMax)
	{
		m_speedMin = m_speedMax;
	}
	else if (m_speedMax < m_speedMin)
	{
		m_speedMax = m_speedMin;
	}

	if (m_lifeTimeMax < 0.01f)
	{
		m_lifeTimeMax = 0.01f;
	}

	if (m_lifeTimeMin > m_lifeTimeMax)
	{
		m_lifeTimeMin = m_lifeTimeMax;
	}
	else if (m_lifeTimeMax < m_lifeTimeMin)
	{
		m_lifeTimeMax = m_lifeTimeMin;
	}

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
	if (m_loop)
	{
		ResetParticles();
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

void ParticleSystem::ResetParticles()
{
	for (int i = 0; i < m_maxParticles; i++)
	{
		ResetParticle(m_particles[i], true);
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
		const glm::mat4x4& matrix = GetTransform()->GetTransformationMatrix();

		// Bottom vertex
		Vector3 v1 = matrix * (glm::vec4(-m_boxSize.x, -m_boxSize.y, -m_boxSize.z, 2) / 2.0f);
		Vector3 v2 = matrix * (glm::vec4(-m_boxSize.x, -m_boxSize.y, m_boxSize.z, 2) / 2.0f);
		Vector3 v3 = matrix * (glm::vec4(m_boxSize.x, -m_boxSize.y, -m_boxSize.z, 2) / 2.0f);
		Vector3 v4 = matrix * (glm::vec4(m_boxSize.x, -m_boxSize.y, m_boxSize.z, 2) / 2.0f);

		// Top vertex
		Vector3 v5 = matrix * (glm::vec4(-m_boxSize.x, m_boxSize.y, -m_boxSize.z, 2) / 2.0f);
		Vector3 v6 = matrix * (glm::vec4(-m_boxSize.x, m_boxSize.y, m_boxSize.z, 2) / 2.0f);
		Vector3 v7 = matrix * (glm::vec4(m_boxSize.x, m_boxSize.y, -m_boxSize.z, 2) / 2.0f);
		Vector3 v8 = matrix * (glm::vec4(m_boxSize.x, m_boxSize.y, m_boxSize.z, 2) / 2.0f);

		v1.x = -v1.x;
		v2.x = -v2.x;
		v3.x = -v3.x;
		v4.x = -v4.x;

		v5.x = -v5.x;
		v6.x = -v6.x;
		v7.x = -v7.x;
		v8.x = -v8.x;

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
	else if (m_emitterShape == EmitterShape::Cone)
	{
		Gizmo::DrawLine(pos, pos + GetTransformRaw()->GetUp() * 3);
	}
#endif
}

int currentFrame = 0;

void ParticleSystem::ResetParticle(Particle& particle, bool setIsDead)
{
	glm::vec3 direction;
	if (m_emitterShape == EmitterShape::Cone)
	{
		particle.position = Vector3(0);
		direction = glm::vec3((rand() % 2000 - 1000) / 1000.0f * m_coneAngle / 180.0f, (rand() % 1000) / 1000.0f + (180 - m_coneAngle) / 180.0f, (rand() % 2000 - 1000) / 1000.0f * m_coneAngle / 180.0f);
		direction = glm::normalize(direction);
	}
	else if (m_emitterShape == EmitterShape::Box)
	{
		particle.position = Vector3(m_boxXDistribution(m_gen), m_boxYDistribution(m_gen), m_boxZDistribution(m_gen));
		direction = glm::vec3(m_direction.x, m_direction.y, m_direction.z);
	}

	if (m_worldSimulation)
	{
		const Quaternion& objectRotation = GetTransformRaw()->GetRotation();
		const Vector3& scale = GetTransformRaw()->GetScale();
		const glm::quat glmObjectRotation = glm::quat(objectRotation.w, objectRotation.x, -objectRotation.y, -objectRotation.z);

		// Fix particle speed
		direction *= glm::vec3(scale.x, scale.y, scale.z);
		// Apply the object rotation to the direction
		particle.direction = glmObjectRotation * direction;
	}
	else
	{
		particle.direction = direction;
	}
	if (m_randomRotation)
	{
		particle.billboardRotation = static_cast<float>(rand() % 360);
	}
	particle.currentSpeed = m_speedDistribution(m_gen);

	particle.currentLifeTime = 0;

	particle.lifeTime = m_lifeTimeDistribution(m_gen);
	particle.isDead = setIsDead;

	static const Quaternion identity = Quaternion::Identity();
	const Quaternion rotation = m_randomRotation ? Quaternion((rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f) : identity;

	if (m_worldSimulation)
	{
		const glm::mat4& transMat = GetTransformRaw()->GetTransformationMatrix();
		particle.matrix = Math::MultiplyMatrices(transMat, Math::CreateModelMatrix(particle.position, rotation, Vector3(1)));
	}
	else
	{
		particle.matrix = Math::CreateModelMatrix(particle.position, rotation, Vector3(1));
	}

	particle.scale = scaleOverLifeTimeFunction(0);

	//particle.frameBeforeUpdate = currentFrame + 1;
	if (currentFrame == 0)
	{
		if (m_simulationRate == SimulationRate::fps_30)
		{
			particle.simulationCooldown = 1 / 30.0f;
		}
		else if (m_simulationRate == SimulationRate::fps_60)
		{
			particle.simulationCooldown = 1 / 60.0f;
		}
		else
		{
			particle.simulationCooldown = 0;
		}
	}
	else
	{
		particle.simulationCooldown = 0;
	}

	currentFrame++;
	currentFrame %= 2;
}

void ParticleSystem::AllocateParticlesMemory()
{
	m_particles.clear();
	for (int i = 0; i < m_maxParticles; i++)
	{
		Particle& newParticle = m_particles.emplace_back();
		if (GetTransformRaw())
		{
			ResetParticle(newParticle, true);
		}
	}
}

float ParticleSystem::DefaultGetScaleOverLifeTime(float lifeTime)
{
	return std::sin(lifeTime * Math::PI);
}

float ParticleSystem::DefaultGetSpeedOverLifeTime(float lifeTime)
{
	return 1;
}

Vector4 ParticleSystem::DefaultGetColorOverLifeTime(float lifeTime)
{
	return Vector4(1, 1, 1, std::sin(lifeTime * Math::PI));
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

	const Vector3& camScale = Graphics::usedCamera->GetTransformRaw()->GetScale();
	const glm::mat4& camMat = Graphics::usedCamera->GetTransformRaw()->GetTransformationMatrix();
	const glm::mat4& transMat = GetTransformRaw()->GetTransformationMatrix();

	const RGBA& rgba = m_color.GetRGBA();
	const Vector3& scale = GetTransformRaw()->GetScale();
	const glm::vec3 fixedScale = glm::vec3(1.0f / camScale.x, 1.0f / camScale.z, 1.0f / camScale.y) * glm::vec3(scale.x, scale.y, scale.z);

	glm::mat4 newMat;
	for (int i = 0; i < m_maxParticles; i++)
	{
		const Particle& particle = m_particles[i];
		if (particle.isDead)
		{
			continue;
		}

		newMat = particle.matrix;

		if (!m_worldSimulation)
		{
			newMat = Math::MultiplyMatrices(transMat, newMat);
		}
		if (m_isBillboard)
		{
			// Copy camera rotation matrix
			newMat[0] = camMat[0];
			newMat[1] = camMat[1];
			newMat[2] = camMat[2];
			// Apply the object rotation on Z
			newMat = glm::rotate(newMat, particle.billboardRotation, glm::vec3(0, 0, 1));
			// Fix scale if the camera has a scale (Y and Z are inverted for some raison)
			newMat = glm::scale(newMat, fixedScale);
		}

		newMat = glm::scale(newMat, glm::vec3(particle.scale));
		renderCommand.subMesh->m_meshData->unifiedColor.SetFromRGBAFloat(rgba.r, rgba.g, rgba.b, std::sin((particle.currentLifeTime / particle.lifeTime) * Math::PI));

#if defined(__PSP__)
		Graphics::DrawSubMesh(*renderCommand.subMesh, *m_material, m_texture.get(), renderSettings, newMat, newMat, newMat, false);
#else
		const glm::mat4 MVP = Graphics::usedCamera->m_viewProjectionMatrix * newMat;
		Graphics::DrawSubMesh(*renderCommand.subMesh, *m_material, m_texture.get(), renderSettings, newMat, newMat, MVP, false);
#endif
	}
}

void ParticleSystem::OnNewRender(int cameraIndex)
{
	// Only update particles for the render
	if (cameraIndex != 0)
	{
		return;
	}
#if defined(EDITOR)
	if (GameplayManager::GetGameState() == GameState::Paused)
	{
		return;
	}
#endif

	float simulationTimer = 0;
	float simulationSpeedMultiplier = 1;
	if(m_simulationRate == SimulationRate::fps_30)
	{
		simulationTimer = 1 / 30.0f;
		simulationSpeedMultiplier = simulationTimer;
	}
	else if (m_simulationRate == SimulationRate::fps_60)
	{
		simulationTimer = 1 / 60.0f;
		simulationSpeedMultiplier = simulationTimer;
	}
	else
	{
		simulationTimer = 0;
		simulationSpeedMultiplier = Time::GetDeltaTime();
	}
	if (Time::GetDeltaTime() > simulationTimer)
	{
		simulationTimer = Time::GetDeltaTime();
		simulationSpeedMultiplier = simulationTimer;
	}

	for (int i = 0; i < m_maxParticles; i++)
	{
		Particle& particle = m_particles[i];
		if (particle.isDead)
		{
			// Spawn new particles
			if (m_isEmitting && m_loop && m_timer >= 1)
			{
				ResetParticle(particle, false);
				m_timer -= 1;
			}
			continue;
		}

		particle.simulationCooldown -= Time::GetUnscaledDeltaTime();
		if (particle.simulationCooldown <= 0)
		{
			particle.matrix[3] += glm::vec4(glm::vec3(particle.direction.x, particle.direction.y, particle.direction.z), 0.0f) * simulationSpeedMultiplier * particle.currentSpeed * particle.speedMultiplier * 1.0f;

			particle.scale = scaleOverLifeTimeFunction(particle.currentLifeTime / particle.lifeTime);
			particle.speedMultiplier = speedMultiplierOverLifeTimeFunction(particle.currentLifeTime / particle.lifeTime);
			//particle.matrix *= glm::toMat4(glm::quat(particle.rotation.w, particle.rotation.x, -particle.rotation.y, -particle.rotation.z));

			particle.simulationCooldown += simulationTimer;
		}

		particle.currentLifeTime += Time::GetDeltaTime();
		if (particle.currentLifeTime >= particle.lifeTime)
		{
			particle.isDead = true;
		}
	}

	if (m_isEmitting && m_loop)
	{
		m_timer += Time::GetDeltaTime() * m_spawnRate;
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
		command.subMesh = SpriteManager::GetBasicSpriteMeshDataWithNormals()->m_subMeshes[0].get();
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
