#include "engine.h"
#include "ui/ui_manager.h"
#include "file_system/file.h"
#include "inputs/input_system.h"
#include "scene_manager/scene.h"
#include "ui/window.h"
#include "gameobject.h"
#include "game.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include "asset_manager.h"
#include "engine_settings.h"

PointLight pointLight = PointLight();
SpotLight spotLight = SpotLight();
SpotLight spotLight2 = SpotLight();
DirectionalLight directionalLight = DirectionalLight();
GameObject pointLightGameObject = GameObject();
GameObject spotLightGameObject = GameObject();
GameObject spotLight2GameObject = GameObject();
GameObject directionalLightGameObject = GameObject();

int Engine::Init()
{
	/* Initialize the library */
	File::InitFileSystem();
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();

	
	pointLight.SetGameObject(&pointLightGameObject);
	pointLight.type = Light::Point;
	pointLight.gameObject->transform.position = Vector3(1.5f, 1.5, 1.5f);
	pointLight.color = Vector3(1, 0.1f, 0.1f);
	pointLight.intensity = 10;
	pointLight.SetRange(7);

	
	spotLight.SetGameObject(&spotLightGameObject);
	spotLight.type = Light::Spot;
	spotLight.gameObject->transform.position = Vector3(0, 3, 0);
	spotLight.gameObject->transform.rotation = Vector3(0.0f, -1.0f, 0.0f);
	spotLight.color = Vector3(0.05f, 0.05f, 1);
	spotLight.intensity = 200;
	spotLight.SetRange(7);
	spotLight.cutOff = glm::cos(glm::radians(12.0f));
	spotLight.outerCutOff = glm::cos(glm::radians(17.0f));

	spotLight2.SetGameObject(&spotLight2GameObject);
	spotLight2.type = Light::Spot;
	spotLight2.gameObject->transform.position = Vector3(5, 3, 0);
	spotLight2.gameObject->transform.rotation = Vector3(0.0f, -1.0f, 0.0f);
	spotLight2.color = Vector3(0.05f, 0.05f, 1);
	spotLight2.intensity = 200;
	spotLight2.SetRange(7);
	spotLight2.cutOff = glm::cos(glm::radians(12.0f));
	spotLight2.outerCutOff = glm::cos(glm::radians(17.0f));
	
	directionalLight.SetGameObject(&directionalLightGameObject);
	directionalLight.type = Light::Directional;
	directionalLight.color = Vector3(0.3f, 0.7f, 0.3f);
	directionalLight.intensity = 1;
	directionalLight.gameObject->transform.rotation = Vector3(0.0f, -1.0f, -1.0f);

	return 0;
}

void Engine::Loop()
{
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

		//Clear the OpenGL window
		glClearColor(0.529f, 0.808f, 0.922f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Material
		AssetManager::shaders[0]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[0]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.position);
		AssetManager::shaders[0]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[0]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[0]->SetShaderAttribut("material.shininess", 32.0f);

		AssetManager::shaders[1]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[1]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.position);
		AssetManager::shaders[1]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[1]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[1]->SetShaderAttribut("material.shininess", 32.0f);

		//Point light
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].color", pointLight.color * pointLight.intensity);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].position", pointLight.gameObject->transform.position);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].linear", pointLight.linear);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].quadratic", pointLight.quadratic);

		//Spot light
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].color", spotLight.intensity * spotLight.color);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].position", spotLight.gameObject->transform.position);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].direction", spotLight.gameObject->transform.rotation);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].linear", spotLight.linear);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].quadratic", spotLight.quadratic);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].cutOff", spotLight.cutOff);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].outerCutOff", spotLight.outerCutOff);

		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].color", spotLight2.intensity * spotLight2.color);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].position", spotLight2.gameObject->transform.position);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].direction", spotLight2.gameObject->transform.rotation);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].linear", spotLight2.linear);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].quadratic", spotLight2.quadratic);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].cutOff", spotLight2.cutOff);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].outerCutOff", spotLight2.outerCutOff);

		//Directional light
		AssetManager::shaders[1]->SetShaderAttribut("directionalLights[0].color", directionalLight.color);
		AssetManager::shaders[1]->SetShaderAttribut("directionalLights[0].direction", directionalLight.gameObject->transform.rotation);

		//Point light
		/*shader.SetShaderAttribut("pointLights[0].color", pointLight.color * pointLight.intensity);
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
		shader2.SetShaderAttribut("offsetPosition", Vector3(animation - 0.5f, 0, 0));*/

		/*if (InputSystem::GetKey(UP)) {
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
		}*/

		//Set wireframe
		if (EngineSettings::isWireframe) {
			glLineWidth(2);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		AssetManager::shaders[3]->SetShaderProjection2D();

		Graphics::DrawAllMesh();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");
		debugText += std::string(", Delta Time: ") + std::to_string(EngineSettings::deltaTime);
		UiManager::RenderText((*AssetManager::shaders[3]), debugText, 0.0f, 720.0f - 24, 0.5f, glm::vec3(0.5f, 0.0f, 0.2f));
		Window::UpdateScreen();
		//frameCount++;
	}
}
