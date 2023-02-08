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
#include "graphics/sprite_manager.h"

PointLight * pointLight = new PointLight();
SpotLight * spotLight = new SpotLight();
SpotLight * spotLight2 = new SpotLight();
DirectionalLight * directionalLight = new DirectionalLight();
GameObject * pointLightGameObject = new GameObject();
GameObject * spotLightGameObject = new GameObject();
GameObject * spotLight2GameObject = new GameObject();
GameObject  *directionalLightGameObject = new GameObject();

std::vector<GameObject*> Engine::gameObjects;
float lastTick = 0;

int Engine::Init()
{
	/* Initialize the library */
	File::InitFileSystem();
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}
	InputSystem::Init();
	SpriteManager::Init();
	
	pointLightGameObject->AddComponent(pointLight);
	pointLight->type = Light::Point;
	pointLight->gameObject->transform.SetPosition(Vector3(1.5f, 1.5, 1.5f));
	pointLight->color = Vector3(1, 0.1f, 0.1f);
	pointLight->intensity = 10;
	pointLight->SetRange(7);

	
	spotLightGameObject->AddComponent(spotLight);
	spotLight->type = Light::Spot;
	spotLight->gameObject->transform.SetPosition(Vector3(0, 3, 0));
	spotLight->gameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLight->color = Vector3(0.05f, 0.05f, 1);
	spotLight->intensity = 200;
	spotLight->SetRange(7);
	spotLight->cutOff = glm::cos(glm::radians(12.0f));
	spotLight->outerCutOff = glm::cos(glm::radians(17.0f));

	spotLight2GameObject->AddComponent(spotLight2);
	spotLight2->type = Light::Spot;
	spotLight2->gameObject->transform.SetPosition(Vector3(5, 3, 0));
	spotLight2->gameObject->transform.SetRotation(Vector3(0.0f, -1.0f, 0.0f));
	spotLight2->color = Vector3(0.05f, 0.05f, 1);
	spotLight2->intensity = 200;
	spotLight2->SetRange(7);
	spotLight2->cutOff = glm::cos(glm::radians(12.0f));
	spotLight2->outerCutOff = glm::cos(glm::radians(17.0f));
	
	directionalLightGameObject->AddComponent(directionalLight);
	directionalLight->type = Light::Directional;
	directionalLight->color = Vector3(0.3f, 0.7f, 0.3f);
	directionalLight->intensity = 1;
	directionalLight->gameObject->transform.SetRotation(Vector3(0.0f, -1.0f, -1.0f));

	return 0;
}

void Engine::Loop()
{
	bool running = true;
	while (running)
	{
		float tempTick = (float)SDL_GetTicks64();
		EngineSettings::deltaTime = (tempTick - lastTick) / 1000.0f;
		lastTick = tempTick;

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
		int gameObjectCount = gameObjects.size();
		for (int gIndex = 0; gIndex < gameObjectCount; gIndex++)
		{
			int componentCount = gameObjects[gIndex]->components.size();
			for (int cIndex = 0; cIndex < componentCount; cIndex++)
			{
				gameObjects[gIndex]->components[cIndex]->Update();
			}
		}

		Game::Loop();

		//Clear the OpenGL window
		glClearColor(0.529f, 0.808f, 0.922f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Material
		AssetManager::shaders[0]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[0]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[0]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[0]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[0]->SetShaderAttribut("material.shininess", 32.0f);

		AssetManager::shaders[1]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[1]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[1]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[1]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[1]->SetShaderAttribut("material.shininess", 32.0f);

		AssetManager::shaders[5]->SetShaderAttribut("ambiantLightColor", Vector3(1, 1, 1));
		AssetManager::shaders[5]->SetShaderAttribut("cameraPos", Graphics::usedCamera->gameObject->transform.GetPosition());
		AssetManager::shaders[5]->SetShaderAttribut("offsetPosition", Vector3(0, 0, 0));
		AssetManager::shaders[5]->SetShaderAttribut("material.ambient", Vector3(0.529f, 0.808f, 0.922f));
		AssetManager::shaders[5]->SetShaderAttribut("material.shininess", 32.0f);

		//Point light
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].color", pointLight->color * pointLight->intensity);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].position", pointLight->gameObject->transform.GetPosition());
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].linear", pointLight->linear);
		AssetManager::shaders[1]->SetShaderAttribut("pointLights[0].quadratic", pointLight->quadratic);

		//Spot light
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].color", spotLight->intensity * spotLight->color);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].position", spotLight->gameObject->transform.GetPosition());
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].direction", spotLight->gameObject->transform.GetRotation());
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].linear", spotLight->linear);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].quadratic", spotLight->quadratic);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].cutOff", spotLight->cutOff);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[0].outerCutOff", spotLight->outerCutOff);

		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].color", spotLight2->intensity * spotLight2->color);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].position", spotLight2->gameObject->transform.GetPosition());
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].direction", spotLight2->gameObject->transform.GetRotation());
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].constant", lightConstant);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].linear", spotLight2->linear);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].quadratic", spotLight2->quadratic);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].cutOff", spotLight2->cutOff);
		AssetManager::shaders[1]->SetShaderAttribut("spotLights[1].outerCutOff", spotLight2->outerCutOff);

		//Directional light
		AssetManager::shaders[1]->SetShaderAttribut("directionalLights[0].color", directionalLight->color);
		AssetManager::shaders[1]->SetShaderAttribut("directionalLights[0].direction", directionalLight->gameObject->transform.GetRotation());

		//Set wireframe
		if (EngineSettings::isWireframe) {
			glLineWidth(2);
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT, GL_FILL);
		}


		Graphics::DrawAllMesh();

		glPolygonMode(GL_FRONT, GL_FILL);

		AssetManager::shaders[3]->SetShaderProjection2D();
		AssetManager::shaders[4]->SetShaderProjection2D();
		//SpriteManager::RenderSprite(AssetManager::shaders[4], 0, 0, AssetManager::textures[6]);
		std::string debugText = std::string("Wireframe (A): ") + (EngineSettings::isWireframe ? "True" : "False");
		debugText += std::string(", Delta Time: ") + std::to_string(EngineSettings::deltaTime);
		debugText += std::string(" ") + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().x) + " " + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().y) + " " + std::to_string(Graphics::usedCamera->gameObject->transform.GetRotation().z);
		UiManager::RenderText((*AssetManager::shaders[3]), debugText, 0.0f, 24, 0.5f, glm::vec3(0.5f, 0.0f, 0.2f));
		Window::UpdateScreen();
		//frameCount++;
	}
}

void Engine::AddGameObject(GameObject* gameObject) {
	gameObjects.push_back(gameObject);
}

std::vector<GameObject*> Engine::GetGameObjects()
{
	return std::vector<GameObject*>(gameObjects);
}
