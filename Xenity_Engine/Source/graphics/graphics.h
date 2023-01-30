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

//#define STB_IMAGE_IMPLEMENTATION

//#include "stb_image.h"
#include "camera.h"
#include "texture.h"

class Graphics
{
public :
	static Camera* usedCamera;
	static std::vector<Mesh*> allMesh;
	static void DrawAllMesh();
};
