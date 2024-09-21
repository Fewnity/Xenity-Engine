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

std::map<int, WorldPartitionner::XNode> WorldPartitionner::Tree::children;

struct Vector3Fast
{
	float x, y, z;
};

// Fonction pour vérifier si un cube intersecte une sphère
bool cubeIntersectsSphere(const Vector3Fast& cubeMin, int cubeSize, const Vector3Fast& sphereCenter, float sphereRadius) {
	float dmin = 0.0f;

	// Calcul de la distance minimale au cube
	for (int i = 0; i < 3; ++i)
	{
		float cubeMaxCoord = (&cubeMin.x)[i] + cubeSize;
		float sphereCoord = (&sphereCenter.x)[i];
		if (sphereCoord < (&cubeMin.x)[i])
		{
			dmin += std::pow(sphereCoord - (&cubeMin.x)[i], 2);
		}
		else if (sphereCoord > cubeMaxCoord)
		{
			dmin += std::pow(sphereCoord - cubeMaxCoord, 2);
		}
	}

	// Si la distance est inférieure au rayon de la sphère, il y a intersection
	return dmin <= sphereRadius * sphereRadius;
}

// Fonction pour obtenir la liste des cubes traversés par la sphère
void getCubesIntersectedBySphere(std::vector<Vector3Fast>& intersectedCubes, const Vector3Fast& pos, float r, int cubeSize) {

	// Détermination des limites de la grille à vérifier
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

	// Parcours de tous les cubes potentiellement concernés
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
	Tree::children.clear();
}

void WorldPartitionner::RemoveMeshRenderer(MeshRenderer* meshRenderer)
{
	XASSERT(meshRenderer, "The meshRenderer is null");

	for (auto& position : meshRenderer->worldChunkPositions)
	{
		const int x = static_cast<int>(position.x / CHUNK_SIZE);
		const int y = static_cast<int>(position.y / CHUNK_SIZE);
		const int z = static_cast<int>(position.z / CHUNK_SIZE);

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

	meshRenderer->worldChunkPositions.clear();
	meshRenderer->affectedByLights.clear();
}

void WorldPartitionner::RemoveLight(Light* light)
{
	XASSERT(light, "The light is null");

	for (auto& position : light->worldChunkPositions)
	{
		const int x = static_cast<int>(position.x / CHUNK_SIZE);
		const int y = static_cast<int>(position.y / CHUNK_SIZE);
		const int z = static_cast<int>(position.z / CHUNK_SIZE);

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
			auto it = std::find(meshRenderer->affectedByLights.begin(), meshRenderer->affectedByLights.end(), light);
			if (it != meshRenderer->affectedByLights.end())
			{
				meshRenderer->affectedByLights.erase(it);
			}
		}
	}

	light->worldChunkPositions.clear();
}

void WorldPartitionner::ProcessMeshRenderer(MeshRenderer* meshRenderer)
{
	XASSERT(meshRenderer, "The meshRenderer is null");

	RemoveMeshRenderer(meshRenderer);

	const Sphere& sphere = meshRenderer->GetBoundingSphere();
	if (sphere.radius == 0)
		return;

	std::vector<Vector3Fast> intersectedCubes;
	getCubesIntersectedBySphere(intersectedCubes, Vector3Fast(sphere.position.x, sphere.position.y, sphere.position.z), sphere.radius, CHUNK_SIZE);

	for (const Vector3Fast& cube : intersectedCubes)
	{
		const int x = static_cast<int>(cube.x / CHUNK_SIZE);
		const int y = static_cast<int>(cube.y / CHUNK_SIZE);
		const int z = static_cast<int>(cube.z / CHUNK_SIZE);

		XNode& xNode = Tree::children[x];
		YNode& yNode = xNode.children[y];
		ZNode& zNode = yNode.children[z];
		Chunk& chunk = zNode.chunk;
		chunk.meshes.push_back(meshRenderer);

		meshRenderer->worldChunkPositions.push_back(Vector3(cube.x, cube.y, cube.z));

		// Add light to mesh
		for (Light* light : chunk.lights)
		{
			// Add the light if it's not already in the list
			auto it = std::find(meshRenderer->affectedByLights.begin(), meshRenderer->affectedByLights.end(), light);

			if (it == meshRenderer->affectedByLights.end())
			{
				meshRenderer->affectedByLights.push_back(light);
			}
		}
	}
}

void WorldPartitionner::ProcessLight(Light* light)
{
	XASSERT(light, "The light is null");

	if (light->GetType() == LightType::Point || light->GetType() == LightType::Spot)
	{
		RemoveLight(light);
		if (light->IsEnabled() && light->GetGameObject()->IsLocalActive())
		{
			std::vector<Vector3Fast> intersectedCubes;
			Sphere sphere;
			sphere.position = light->GetTransform()->GetPosition();
			sphere.radius = light->GetMaxLightDistance();
			getCubesIntersectedBySphere(intersectedCubes, Vector3Fast(sphere.position.x, sphere.position.y, sphere.position.z), sphere.radius, CHUNK_SIZE);
			for (const Vector3Fast& cube : intersectedCubes)
			{
				const int x = static_cast<int>(cube.x / CHUNK_SIZE);
				const int y = static_cast<int>(cube.y / CHUNK_SIZE);
				const int z = static_cast<int>(cube.z / CHUNK_SIZE);

				XNode& xNode = Tree::children[x];
				YNode& yNode = xNode.children[y];
				ZNode& zNode = yNode.children[z];
				Chunk& chunk = zNode.chunk;
				chunk.lights.push_back(light);

				light->worldChunkPositions.push_back(Vector3(cube.x, cube.y, cube.z));

				for (MeshRenderer* meshRenderer : chunk.meshes)
				{
					auto it = std::find(meshRenderer->affectedByLights.begin(), meshRenderer->affectedByLights.end(), light);
					if (it == meshRenderer->affectedByLights.end())
					{
						meshRenderer->affectedByLights.push_back(light);
					}
				}
			}
		}
	}
}

void WorldPartitionner::DrawChunk(const Chunk& chunk, int x, int y, int z)
{
	if (chunk.meshes.empty() && chunk.lights.empty())
		return;

#if defined(EDITOR)
	const Vector3 pos = Vector3(static_cast<float>(x * CHUNK_SIZE), static_cast<float>(y * CHUNK_SIZE), static_cast<float>(z * CHUNK_SIZE));

	// Bottom vertex
	const Vector3 v1 = pos + Vector3(-CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v2 = pos + Vector3(-CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v3 = pos + Vector3(CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v4 = pos + Vector3(CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);

	// Top vertex
	const Vector3 v5 = pos + Vector3(-CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v6 = pos + Vector3(-CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v7 = pos + Vector3(CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);
	const Vector3 v8 = pos + Vector3(CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, CHUNK_HALF_SIZE) + Vector3(CHUNK_HALF_SIZE);

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
