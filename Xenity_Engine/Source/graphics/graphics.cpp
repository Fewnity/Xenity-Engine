#include "graphics.h"
#include "../asset_manager.h"

Camera* Graphics::usedCamera = nullptr;
//std::vector<Mesh*> Graphics::allMesh;

void Graphics::DrawAllMesh()
{
	for (Mesh* mesh : AssetManager::meshs)
	{
		mesh->DrawModel();
	}
}