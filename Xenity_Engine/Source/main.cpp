#include "engine.h"
#include "main.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include <iostream>

#include "gameobject.h"
#include "ui/window.h"

#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"

#include "game.h"

#undef main

using namespace std;

/*
 * TODO :
 * Sounds
 * Fix normals when models doesn't have normals
 * Fix transparency order
 * Better engine structure (lighting)
 * New light type (square light)
 */

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

bool wireframe = false;
float animation = 0;

void GameLoop();

int main(void)
{
	Engine::Init();
	Engine::Loop();

	/* Initialize the library */
	File::InitFileSystem();
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();

	Game::Init();

	bool running = true;
	while (running)
	{
		SDL_Event event;
		InputSystem::ClearInputs();
		while (SDL_PollEvent(&event))
		{
			InputSystem::UpdateInputs(event);
			switch (event.type)
			{
			case SDL_QUIT:
				running = false;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					Window::Resize(event.window.data1, event.window.data2);
				}
				break;
			default:
				break;
			}
		}
		Game::Loop();
		//GameLoop();

		//Clear the OpenGL window
		glClearColor(0.529f, 0.808f, 0.922f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Material
		shader.SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		shader.SetShaderAttribut("cameraPos", camera.gameObject->transform.position);
		shader.SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		shader.SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		shader.SetShaderAttribut("material.shininess", 32.0f);

		//Point light
		shader.SetShaderAttribut("pointLights[0].color", pointLight.color * pointLight.intensity);
		shader.SetShaderAttribut("pointLights[0].position", pointLight.gameObject->transform.position);
		shader.SetShaderAttribut("pointLights[0].constant", lightConstant);
		shader.SetShaderAttribut("pointLights[0].linear", pointLight.linear);
		shader.SetShaderAttribut("pointLights[0].quadratic", pointLight.quadratic);

		//Spot light
		shader.SetShaderAttribut("spotLights[0].color", spotLight.intensity * spotLight.color);
		shader.SetShaderAttribut("spotLights[0].position", spotLight.gameObject->transform.position);
		shader.SetShaderAttribut("spotLights[0].direction", spotLight.gameObject->transform.rotation);
		shader.SetShaderAttribut("spotLights[0].constant", lightConstant);
		shader.SetShaderAttribut("spotLights[0].linear", spotLight.linear);
		shader.SetShaderAttribut("spotLights[0].quadratic", spotLight.quadratic);
		shader.SetShaderAttribut("spotLights[0].cutOff", spotLight.cutOff);
		shader.SetShaderAttribut("spotLights[0].outerCutOff", spotLight.outerCutOff);

		//Directional light
		shader.SetShaderAttribut("directionalLights[0].color", directionalLight.color);
		shader.SetShaderAttribut("directionalLights[0].direction", directionalLight.gameObject->transform.rotation);

		//Material
		shader3.SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		shader3.SetShaderAttribut("cameraPos", camera.gameObject->transform.position);
		shader3.SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		shader3.SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		shader3.SetShaderAttribut("material.shininess", 32.0f);

		//Point light
		shader3.SetShaderAttribut("pointLights[0].color", pointLight.color * pointLight.intensity);
		shader3.SetShaderAttribut("pointLights[0].position", pointLight.gameObject->transform.position);
		shader3.SetShaderAttribut("pointLights[0].constant", lightConstant);
		shader3.SetShaderAttribut("pointLights[0].linear", pointLight.linear);
		shader3.SetShaderAttribut("pointLights[0].quadratic", pointLight.quadratic);

		//Spot light
		shader3.SetShaderAttribut("spotLights[0].color", spotLight.intensity * spotLight.color);
		shader3.SetShaderAttribut("spotLights[0].position", spotLight.gameObject->transform.position);
		shader3.SetShaderAttribut("spotLights[0].direction", spotLight.gameObject->transform.rotation);
		shader3.SetShaderAttribut("spotLights[0].constant", lightConstant);
		shader3.SetShaderAttribut("spotLights[0].linear", spotLight.linear);
		shader3.SetShaderAttribut("spotLights[0].quadratic", spotLight.quadratic);
		shader3.SetShaderAttribut("spotLights[0].cutOff", spotLight.cutOff);
		shader3.SetShaderAttribut("spotLights[0].outerCutOff", spotLight.outerCutOff);

		//Directional light
		shader3.SetShaderAttribut("directionalLights[0].color", directionalLight.color);
		shader3.SetShaderAttribut("directionalLights[0].direction", directionalLight.gameObject->transform.rotation);

		shader2.SetShaderAttribut("ourColor", Vector4(1 - animation, 1 - animation, 1 - animation, 0.5));
		shader2.SetShaderAttribut("offsetPosition", Vector3(animation - 0.5f, 0, 0));

		if (InputSystem::GetKey(UP)) {
			pointLight.SetRange(pointLight.GetRange() + 0.2f);
		}
		else if (InputSystem::GetKey(DOWN)) {
			if (pointLight.GetRange() > 0)
				pointLight.SetRange(pointLight.GetRange() - 0.2f);
		}
		if (InputSystem::GetKey(LEFT)) {
			spotLight.SetRange(spotLight.GetRange() + 0.2f);
		}
		else if (InputSystem::GetKey(RIGHT)) {
			if (spotLight.GetRange() > 0)
				spotLight.SetRange(spotLight.GetRange() - 0.2f);
		}

		//Draw
		//mesh1.position.x = animation;
		//mesh1.rotation.y = (float)SDL_GetTicks64() / 8.726646259971623f;
		//mesh1.rotation.y = 90;

		shaderText.SetShaderProjection2D();

		mesh3.gameObject->transform.rotation.y = 45;
		mesh4.gameObject->transform.rotation.y -= 0.1f;
		mesh5.gameObject->transform.scale = Vector3(10, 1, 10);

		//Set wireframe
		if (wireframe) {
			glLineWidth(2);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		Graphics::DrawAllMesh();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		std::string wireframeText = std::string("Wireframe (A): ") + (wireframe ? "True" : "False");
		UiManager::RenderText(shaderText, wireframeText, 0.0f, 720.0f - 24, 0.5f, glm::vec3(0.5f, 0.0f, 0.2f));
		Window::UpdateScreen();
		frameCount++;
	}

	glfwTerminate();
	return 0;
}

void GameLoop()
{
	if (InputSystem::GetKeyDown(ESCAPE))
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	if (InputSystem::GetKeyDown(A))
	{
		wireframe = !wireframe;
	}

	if (InputSystem::GetKeyDown(W))
	{
		myGameObject1.active = !myGameObject1.active;
	}
	if (InputSystem::GetKeyDown(X))
	{
		myGameObject2.active = !myGameObject2.active;
	}
	if (InputSystem::GetKeyDown(C))
	{
		myGameObject3.active = !myGameObject3.active;
	}

	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect /= 20.0f;
		camera.gameObject->transform.position.x += vect.x;
		camera.gameObject->transform.position.y += vect.y;
		camera.gameObject->transform.position.z += vect.z;
	}
	if (InputSystem::GetKey(S)) {
		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate();
		vect /= 20.0f;
		camera.gameObject->transform.position.x -= vect.x;
		camera.gameObject->transform.position.y -= vect.y;
		camera.gameObject->transform.position.z -= vect.z;
	}
	if (InputSystem::GetKey(D)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect /= 20.0f;
		camera.gameObject->transform.position.x += vect.x;
		camera.gameObject->transform.position.z += vect.z;
	}
	if (InputSystem::GetKey(Q)) {

		Vector3 vect = Graphics::usedCamera->GetSphericalCoordinate2();
		vect /= 20.0f;
		camera.gameObject->transform.position.x -= vect.x;
		camera.gameObject->transform.position.z -= vect.z;
	}

	//Animation
	animation = (float)SDL_GetTicks64() / 500;
	animation = sin(animation) / 2.0f + 0.5f;

	if (camera.gameObject->transform.rotation.x + -InputSystem::mouseSpeed.y / 4.0f < 90 && camera.gameObject->transform.rotation.x + -InputSystem::mouseSpeed.y / 4.0f > -90)
		camera.gameObject->transform.rotation.x += -InputSystem::mouseSpeed.y / 4.0f;

	camera.gameObject->transform.rotation.y += -InputSystem::mouseSpeed.x / 4.0f;
}