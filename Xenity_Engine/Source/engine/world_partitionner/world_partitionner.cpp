#include "world_partitionner.h"

#if defined(EDITOR)
#include <editor/gizmo.h>
#endif

#include <engine/asset_management/asset_manager.h>
#include <engine/vectors/vector3.h>
#include <engine/lighting/lighting.h>
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/debug/debug.h>
#include <engine/engine.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/tools/math.h>
#include <engine/tools/benchmark.h>
#include <engine/debug/performance.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/constants.h>

std::map<int, WorldPartitionner::XNode> WorldPartitionner::Tree::children;

struct Vector3Fast
{
	Vector3Fast(float x, float y, float z) 
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	float x, y, z;
};

// Fonction pour v�rifier si un cube intersecte une sph�re
bool cubeIntersectsSphere(const Vector3Fast& cubeMin, int cubeSize, const Vector3Fast& sphereCenter, float sphereRadius)
{
	// STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	float dmin = 0.0f;

	// Calcul de la distance minimale au cube
	for (int i = 0; i < 3; ++i)
	{
		float cubeMaxCoord = (&cubeMin.x)[i] + cubeSize;
		float sphereCoord = (&sphereCenter.x)[i];
		if (sphereCoord < (&cubeMin.x)[i])
		{
			dmin += static_cast<float>(std::pow(sphereCoord - (&cubeMin.x)[i], 2));
		}
		else if (sphereCoord > cubeMaxCoord)
		{
			dmin += static_cast<float>(std::pow(sphereCoord - cubeMaxCoord, 2));
		}
	}

	// Si la distance est inf�rieure au rayon de la sph�re, il y a intersection
	return dmin <= sphereRadius * sphereRadius;
}

// Fonction pour obtenir la liste des cubes travers�s par la sph�re
void getCubesIntersectedBySphere(std::vector<Vector3Fast>& intersectedCubes, const Vector3Fast& pos, float r, int cubeSize) 
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);

	// D�termination des limites de la grille � v�rifier
	const Vector3Fast minCube = Vector3Fast(
		std::floor((pos.x - r) / cubeSize) * cubeSize,
		std::floor((pos.y - r) / cubeSize) * cubeSize,
		std::floor((pos.z - r) / cubeSize) * cubeSize
	);

	const Vector3Fast maxCube = Vector3Fast(
		std::floor((pos.x + r) / cubeSize) * cubeSize,
		std::floor((pos.y + r) / cubeSize) * cubeSize,
		std::floor((pos.z + r) / cubeSize) * cubeSize
	);

	// Parcours de tous les cubes potentiellement concern�s
	for (float x = minCube.x; x <= maxCube.x; x += cubeSize)
	{
		for (float y = minCube.y; y <= maxCube.y; y += cubeSize)
		{
			for (float z = minCube.z; z <= maxCube.z; z += cubeSize)
			{
				const Vector3Fast cubeMin = Vector3Fast(x, y, z);
				if (cubeIntersectsSphere(cubeMin, cubeSize, pos, r))
				{
					intersectedCubes.push_back(cubeMin);
				}
			}
		}
	}
}

void WorldPartitionner::ClearWorld()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	Tree::children.clear();
}

void WorldPartitionner::RemoveMeshRenderer(MeshRenderer* meshRenderer)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(meshRenderer, "The meshRenderer is null");

	for (auto& position : meshRenderer->m_worldChunkPositions)
	{
		const int x = static_cast<int>(position.x / WORLD_CHUNK_SIZE);
		const int y = static_cast<int>(position.y / WORLD_CHUNK_SIZE);
		const int z = static_cast<int>(position.z / WORLD_CHUNK_SIZE);

		XNode& xNode = Tree::children[x];
		YNode& yNode = xNode.children[y];
		ZNode& zNode = yNode.children[z];
		Chunk& chunk = zNode.chunk;

		auto it = std::find(chunk.meshes.begin(), chunk.meshes.end(), meshRenderer);
		if (it != chunk.meshes.end())
		{
			chunk.meshes.erase(it);
		}
	}

	meshRenderer->m_worldChunkPositions.clear();
	meshRenderer->m_affectedByLights.clear();
}

void WorldPartitionner::RemoveLight(Light* light)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(light, "The light is null");

	for (auto& position : light->m_worldChunkPositions)
	{
		const int x = static_cast<int>(position.x / WORLD_CHUNK_SIZE);
		const int y = static_cast<int>(position.y / WORLD_CHUNK_SIZE);
		const int z = static_cast<int>(position.z / WORLD_CHUNK_SIZE);

		XNode& xNode = Tree::children[x];
		YNode& yNode = xNode.children[y];
		ZNode& zNode = yNode.children[z];
		Chunk& chunk = zNode.chunk;

		auto it = std::find(chunk.lights.begin(), chunk.lights.end(), light);
		if (it != chunk.lights.end())
		{
			chunk.lights.erase(it);
		}

		for (MeshRenderer* meshRenderer : chunk.meshes)
		{
			// Add the light if it's not already in the list
			auto it = std::find(meshRenderer->m_affectedByLights.begin(), meshRenderer->m_affectedByLights.end(), light);
			if (it != meshRenderer->m_affectedByLights.end())
			{
				meshRenderer->m_affectedByLights.erase(it);
			}
		}
	}

	light->m_worldChunkPositions.clear();
}

void WorldPartitionner::ProcessMeshRenderer(MeshRenderer* meshRenderer)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(meshRenderer, "The meshRenderer is null");

	RemoveMeshRenderer(meshRenderer);

	const Sphere& sphere = meshRenderer->GetBoundingSphere();
	if (sphere.radius == 0)
		return;

	std::vector<Vector3Fast> intersectedCubes;
	getCubesIntersectedBySphere(intersectedCubes, Vector3Fast(sphere.position.x, sphere.position.y, sphere.position.z), sphere.radius, WORLD_CHUNK_SIZE);

	for (const Vector3Fast& cube : intersectedCubes)
	{
		const int x = static_cast<int>(cube.x / WORLD_CHUNK_SIZE);
		const int y = static_cast<int>(cube.y / WORLD_CHUNK_SIZE);
		const int z = static_cast<int>(cube.z / WORLD_CHUNK_SIZE);

		XNode& xNode = Tree::children[x];
		YNode& yNode = xNode.children[y];
		ZNode& zNode = yNode.children[z];
		Chunk& chunk = zNode.chunk;
		chunk.meshes.push_back(meshRenderer);

		meshRenderer->m_worldChunkPositions.push_back(Vector3(cube.x, cube.y, cube.z));

		// Add light to mesh
		for (Light* light : chunk.lights)
		{
			// Add the light if it's not already in the list
			auto it = std::find(meshRenderer->m_affectedByLights.begin(), meshRenderer->m_affectedByLights.end(), light);

			if (it == meshRenderer->m_affectedByLights.end())
			{
				meshRenderer->m_affectedByLights.push_back(light);
			}
		}
	}
}

void WorldPartitionner::ProcessLight(Light* light)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);

	XASSERT(light, "The light is null");

	RemoveLight(light);
	if (light->GetType() == LightType::Point || light->GetType() == LightType::Spot)
	{
		if (light->IsEnabled() && light->GetGameObject()->IsLocalActive())
		{
			std::vector<Vector3Fast> intersectedCubes;
			Sphere sphere;
			sphere.position = light->GetTransform()->GetPosition();
			sphere.radius = light->GetMaxLightDistance();
			getCubesIntersectedBySphere(intersectedCubes, Vector3Fast(sphere.position.x, sphere.position.y, sphere.position.z), sphere.radius, WORLD_CHUNK_SIZE);
			for (const Vector3Fast& cube : intersectedCubes)
			{
				const int x = static_cast<int>(cube.x / WORLD_CHUNK_SIZE);
				const int y = static_cast<int>(cube.y / WORLD_CHUNK_SIZE);
				const int z = static_cast<int>(cube.z / WORLD_CHUNK_SIZE);

				XNode& xNode = Tree::children[x];
				YNode& yNode = xNode.children[y];
				ZNode& zNode = yNode.children[z];
				Chunk& chunk = zNode.chunk;
				chunk.lights.push_back(light);

				light->m_worldChunkPositions.push_back(Vector3(cube.x, cube.y, cube.z));

				for (MeshRenderer* meshRenderer : chunk.meshes)
				{
					auto it = std::find(meshRenderer->m_affectedByLights.begin(), meshRenderer->m_affectedByLights.end(), light);
					if (it == meshRenderer->m_affectedByLights.end())
					{
						meshRenderer->m_affectedByLights.push_back(light);
					}
				}
			}
		}
	}
}

void WorldPartitionner::DrawChunk(const Chunk& chunk, int x, int y, int z)
{
	STACK_DEBUG_OBJECT(STACK_VERY_LOW_PRIORITY);

	if (chunk.meshes.empty() && chunk.lights.empty())
		return;

#if defined(EDITOR)
	const Vector3 pos = Vector3(static_cast<float>(x * WORLD_CHUNK_SIZE), static_cast<float>(y * WORLD_CHUNK_SIZE), static_cast<float>(z * WORLD_CHUNK_SIZE));

	// Bottom vertex
	const Vector3 v1 = pos + Vector3(-WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v2 = pos + Vector3(-WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v3 = pos + Vector3(WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v4 = pos + Vector3(WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);

	// Top vertex
	const Vector3 v5 = pos + Vector3(-WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v6 = pos + Vector3(-WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v7 = pos + Vector3(WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE, -WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);
	const Vector3 v8 = pos + Vector3(WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE, WORLD_CHUNK_HALF_SIZE) + Vector3(WORLD_CHUNK_HALF_SIZE);

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
#endif
}

void WorldPartitionner::OnDrawGizmos()
{
	STACK_DEBUG_OBJECT(STACK_LOW_PRIORITY);
	return;

#if defined(EDITOR)
	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	const Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 0.2f);

	Gizmo::SetColor(lineColor);

	for (const auto& xNode : Tree::children)
	{
		for (const auto& yNode : xNode.second.children)
		{
			for (const auto& zNode : yNode.second.children)
			{
				DrawChunk(zNode.second.chunk, xNode.first, yNode.first, zNode.first);
			}
		}
	}

#endif
}
