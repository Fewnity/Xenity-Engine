#include "world_partitionner.h"

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

#if defined(EDITOR)
#include <editor/gizmo.h>
#endif

std::map<int, WorldPartitionner::XNode> WorldPartitionner::Tree::children;

void WorldPartitionner::ProcessPartionning()
{
	Benchmark bench = Benchmark();
	bench.Start();
	Tree::children.clear();

	// Get all lights
	const std::vector<std::weak_ptr<Light>>& lights = AssetManager::GetLights();
	// Get all meshes
	const std::vector<std::weak_ptr<IDrawable>>& drawables = Graphics::orderedIDrawable;

	for (const std::weak_ptr<IDrawable>& drawable : drawables)
	{
		// If the drawable is a mesh
		//if (drawables[i].lock()->GetType() == IDrawableType::Mesh)
		const std::shared_ptr<MeshRenderer> meshRenderer = std::dynamic_pointer_cast<MeshRenderer>(drawable.lock());
		if (meshRenderer && meshRenderer->GetMeshData())
		{
			// Get the bounding box of the mesh
			const Vector3 min = meshRenderer->GetMeshData()->GetMinBoundingBox();
			const Vector3 max = meshRenderer->GetMeshData()->GetMaxBoundingBox();

			const Vector3& scale = meshRenderer->GetTransform()->GetScale();
			const float biggestScale = std::max(abs(scale.x), std::max(abs(scale.y), abs(scale.z)));
			const float smallestMin = std::min(min.x, std::min(min.y, min.z)) * biggestScale;
			const float biggestMax = std::max(max.x, std::max(max.y, max.z)) * biggestScale;

			const glm::mat4x4 modelMatrix = Math::CreateModelMatrix(meshRenderer->GetTransform()->GetPosition(), Vector3(0), Vector3(1));
			const glm::vec3 maxMult = modelMatrix * glm::vec4(smallestMin, smallestMin, smallestMin, 1);
			const glm::vec3 minMult = modelMatrix * glm::vec4(biggestMax, biggestMax, biggestMax, 1);

			int cellMinX, cellMaxX, cellMinY, cellMaxY, cellMinZ, cellMaxZ = 0;

			if (maxMult.x < 0 && minMult.x < 0)
			{
				cellMinX = ((int)floor(minMult.x / CHUNK_HALF_SIZE) / 2);
				cellMaxX = (int)(floor((maxMult.x / CHUNK_HALF_SIZE - 1.0f) / 2));
			}
			else
			{
				cellMinX = ((int)ceil(minMult.x / CHUNK_HALF_SIZE) / 2);
				cellMaxX = (int)(floor((maxMult.x / CHUNK_HALF_SIZE - 1) / 2));
			}

			if (maxMult.y < 0 && minMult.y < 0)
			{
				cellMinY = ((int)floor(minMult.y / CHUNK_HALF_SIZE) / 2);
				cellMaxY = (int)(floor((maxMult.y / CHUNK_HALF_SIZE - 1) / 2));
			}
			else
			{
				cellMinY = ((int)ceil(minMult.y / CHUNK_HALF_SIZE) / 2);
				cellMaxY = (int)(floor((maxMult.y / CHUNK_HALF_SIZE - 1) / 2));
			}

			if (maxMult.z < 0 && minMult.z < 0)
			{
				cellMinZ = ((int)floor(minMult.z / CHUNK_HALF_SIZE) / 2);
				cellMaxZ = (int)(floor((maxMult.z / CHUNK_HALF_SIZE - 1) / 2));
			}
			else
			{
				cellMinZ = ((int)ceil(minMult.z / CHUNK_HALF_SIZE) / 2);
				cellMaxZ = (int)(floor((maxMult.z / CHUNK_HALF_SIZE - 1) / 2));
			}

			for (int x = cellMaxX; x < cellMinX; x++)
			{
				const int chunkXPos = -x - 1;
				XNode& xNode = Tree::children[chunkXPos];
				for (int y = cellMaxY; y < cellMinY; y++)
				{
					const int chunkYPos = y + 1;
					YNode& yNode = xNode.children[chunkYPos];
					for (int z = cellMaxZ; z < cellMinZ; z++)
					{
						const int chunkZPos = z + 1;
						ZNode& zNode = yNode.children[chunkZPos];
						Chunk& chunk = zNode.chunk;
						chunk.meshes.push_back(meshRenderer);
					}
				}
			}
		}
	}

	// For each light
	//for (int i = 0; i < lights.size(); i++)
	//{
	//	// Get the light
	//	std::shared_ptr<Light> light = lights[i].lock();

	//	// Get the light position
	//	const Vector3& lightPosition = light->GetGameObject()->GetTransform()->GetPosition();

	//	// Get the chunk position
	//	int chunkX = round(lightPosition.x / CHUNK_SIZE);
	//	int chunkY = round(lightPosition.y / CHUNK_SIZE);
	//	int chunkZ = round(lightPosition.z / CHUNK_SIZE);

	//	// Get the chunk
	//	Chunk* chunk = nullptr;
	//	for (int j = 0; j < chunks.size(); j++)
	//	{
	//		if (chunks[j].x == chunkX && chunks[j].y == chunkY && chunks[j].z == chunkZ)
	//		{
	//			chunk = &chunks[j];
	//			break;
	//		}
	//	}

	//	// If the chunk doesn't exist, create it
	//	if (chunk == nullptr)
	//	{
	//		Chunk newChunk;
	//		newChunk.x = chunkX;
	//		newChunk.y = chunkY;
	//		newChunk.z = chunkZ;
	//		chunks.push_back(newChunk);
	//		chunk = &chunks[chunks.size() - 1];
	//	}

	//	// Add the light to the chunk
	//	chunk->lights.push_back(light);
	//}
	bench.Stop();
	Debug::Print("World partitionning done: " + std::to_string(bench.GetMicroSeconds()));
	//bench.Reset();
	//bench.Start();

	//for (const std::weak_ptr<IDrawable> drawable : drawables)
	//{
	//	// If the drawable is a mesh
	//	//if (drawables[i].lock()->GetType() == IDrawableType::Mesh)
	//	const std::shared_ptr<MeshRenderer> meshRenderer = std::dynamic_pointer_cast<MeshRenderer>(drawable.lock());
	//	if (meshRenderer && meshRenderer->GetMeshData())
	//	{
	//		// Get the bounding box of the mesh
	//		const Vector3 min = meshRenderer->GetMeshData()->GetMinBoundingBox();
	//		const Vector3 max = meshRenderer->GetMeshData()->GetMaxBoundingBox();

	//		const glm::mat4x4 modelMatrix = Math::CreateModelMatrix(meshRenderer->GetGameObject()->GetTransform()->GetPosition(), Vector3(0), Vector3(1));

	//		const Vector3 scale = meshRenderer->GetGameObject()->GetTransform()->GetScale();
	//		const float biggestScale = std::max(abs(scale.x), std::max(abs(scale.y), abs(scale.z)));
	//		const float smallestMin = std::min(min.x, std::min(min.y, min.z)) * biggestScale;
	//		const float biggestMax = std::max(max.x, std::max(max.y, max.z)) * biggestScale;


	//		const glm::vec3 maxMult = modelMatrix * glm::vec4(smallestMin, smallestMin, smallestMin, 1);
	//		const glm::vec3 minMult = modelMatrix * glm::vec4(biggestMax, biggestMax, biggestMax, 1);

	//		int cellMinX, cellMaxX, cellMinY, cellMaxY, cellMinZ, cellMaxZ = 0;

	//		if (maxMult.x < 0 && minMult.x < 0)
	//		{
	//			cellMinX = (int)(floor((minMult.x) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxX = (int)(floor((maxMult.x / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}
	//		else
	//		{
	//			cellMinX = (int)(ceil((minMult.x) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxX = (int)(floor((maxMult.x / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}
	//		if (maxMult.y < 0 && minMult.y < 0)
	//		{
	//			cellMinY = (int)(floor((minMult.y) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxY = (int)(floor((maxMult.y / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}
	//		else
	//		{
	//			cellMinY = (int)(ceil((minMult.y) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxY = (int)(floor((maxMult.y / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}

	//		if (maxMult.z < 0 && minMult.z < 0)
	//		{
	//			cellMinZ = (int)(floor((minMult.z) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxZ = (int)(floor((maxMult.z / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}
	//		else
	//		{
	//			cellMinZ = (int)(ceil((minMult.z) / CHUNK_HALF_SIZE) / 2.0f);
	//			cellMaxZ = (int)(floor((maxMult.z / CHUNK_HALF_SIZE - 1.0f) / 2.0f));
	//		}

	//		for (int x = cellMaxX; x < cellMinX; x++)
	//		{
	//			const int chunkXPos = -x - 1;
	//			XNode& xNode = Tree::children[chunkXPos];
	//			for (int y = cellMaxY; y < cellMinY; y++)
	//			{
	//				const int chunkYPos = y + 1;
	//				YNode& yNode = xNode.children[chunkYPos];
	//				for (int z = cellMaxZ; z < cellMinZ; z++)
	//				{
	//					const int chunkZPos = z + 1;
	//					ZNode& zNode = yNode.children[chunkZPos];
	//					Chunk& chunk = zNode.chunk;
	//					chunk.meshes.push_back(meshRenderer);
	//				}
	//			}
	//		}
	//	}
	//}

	//bench.Stop();
	//Debug::Print("World partitionning done2: " + std::to_string(bench.GetMicroSeconds()));
}

//std::vector<const Chunk*> WorldPartitionner::GetChunksOfMeshRenderer(const std::shared_ptr<MeshRenderer>& meshRenderer)
//{
//	std::vector<const Chunk*> chunks;
//	for (const auto& xNode : Tree::children)
//	{
//		for (const auto& yNode : xNode.second.children)
//		{
//			for (const auto& zNode : yNode.second.children)
//			{
//				for(const std::weak_ptr<MeshRenderer>& mesh : zNode.second.chunk.meshes)
//				{
//					if (mesh.lock() == meshRenderer)
//					{
//						chunks.push_back(&zNode.second.chunk);
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	return chunks;
//}

void WorldPartitionner::DrawChunk(const Chunk& chunk, int x, int y, int z)
{
#if defined(EDITOR)
	const Vector3 pos = Vector3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);

	// Bottom vertex
	const Vector3 v1 = pos + Vector3(-CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE);
	const Vector3 v2 = pos + Vector3(-CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, CHUNK_HALF_SIZE);
	const Vector3 v3 = pos + Vector3(CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE);
	const Vector3 v4 = pos + Vector3(CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE, CHUNK_HALF_SIZE);

	// Top vertex
	const Vector3 v5 = pos + Vector3(-CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE);
	const Vector3 v6 = pos + Vector3(-CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, CHUNK_HALF_SIZE);
	const Vector3 v7 = pos + Vector3(CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, -CHUNK_HALF_SIZE);
	const Vector3 v8 = pos + Vector3(CHUNK_HALF_SIZE, CHUNK_HALF_SIZE, CHUNK_HALF_SIZE);

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
	//return;
#if defined(EDITOR)
	Engine::GetRenderer().SetCameraPosition(*Graphics::usedCamera);

	const Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);

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
