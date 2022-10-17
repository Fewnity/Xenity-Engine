#include "graphics.h"

Camera* Graphics::usedCamera;
std::vector<Mesh*> Graphics::allMesh;

void Graphics::DrawAllMesh()
{
	for(Mesh* mesh : allMesh)
	{
		mesh->DrawModel();
	}
}