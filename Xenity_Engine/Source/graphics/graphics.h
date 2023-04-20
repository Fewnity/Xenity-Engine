#pragma once

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include "shader.h"
#include "mesh.h"

#include "camera.h"
#include "texture.h"

class IDrawable;


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
