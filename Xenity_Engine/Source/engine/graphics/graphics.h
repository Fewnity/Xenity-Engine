#pragma once

#include <vector>

class IDrawable;
class Material;
class Camera;

class Graphics
{
public :
	static Camera* usedCamera;
	static int usedShaderProgram;
	static void DrawAllDrawable();
	static Material* usedMaterial;
	static void OrderDrawables();

	static std::vector<IDrawable*> orderedIDrawable;
	static int iDrawablesCount;

};
