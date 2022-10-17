#include "engine.h"
#include "main.h"
#include "graphics/graphics.h"
#include <SDL2/SDL.h>
#include <iostream>

#include "gameobject.h"
#include "ui/window.h"

#include "ui/ui_manager.h"
#include "file.h"
#include "inputs/input_system.h"

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

void GameLoop();

template <class  T>
void AddComponent2()
{
	cout << sizeof(T);
	T classe = T();
	// ...
}

int main(void)
{
	/* Initialize the library */
	File::InitFileSystem();
	if (Window::InitWindow() != 0 || UiManager::Init() != 0 || Audio::Init() != 0) {
		return -1;
	}

	//AudioClip audio1 = AudioClip(R"(C:\Users\elect\Desktop\Projets Visual Studio\OpenGL_Test\Debug\camera-13695.mp3)");
	//AudioClip audio1 = AudioClip(R"(D:\Projet Visual Studio\OpenGL_Test\Debug\camera-13695.mp3)");
	//AudioClip audio1 = AudioClip(R"(C:\Users\gregory.machefer\Downloads\OpenGL_Test\Debug\camera-13695.mp3)");
	AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	InputSystem::Init();
	GameObject cameraGameObject = GameObject();
	Camera camera = Camera();
	camera.SetGameObject(&cameraGameObject);
	camera.gameObject->transform.position = Vector3(0, 2, 2);
	Graphics::usedCamera = &camera;

	/*float vertices[] = {
		 0.5, 0.5, 0.0,   1.0, 0.0, 0.0, 1.0, 1.0,
		 0.5, -0.5, 0.0,  0.0, 1.0, 0.0, 1.0, 0.0,
		 -0.5, -0.5, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		 -0.5, 0.5, 0.0,  1.0, 0.0, 1.0, 0.0, 1.0,

		 0.5, 0.5, 1.0,   1.0, 0.0, 0.0, 1.0, 1.0,
		 0.5, -0.5, 1.0,  0.0, 1.0, 0.0, 1.0, 0.0,
		 -0.5, -0.5, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		 -0.5, 0.5, 1.0,  1.0, 0.0, 1.0, 0.0, 1.0,
	};*/

	float vertices[] = {
		 0.5, 0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, 0.0, 1.0, 0.0,
		 -0.5, -0.5, 0.0, 0.0, 0.0,
		 -0.5, 0.5, 0.0, 0.0, 1.0,

		 0.5, 0.5, 1.0, 1.0, 1.0,
		 0.5, -0.5, 1.0, 1.0, 0.0,
		 -0.5, -0.5, 1.0, 0.0, 0.0,
		 -0.5, 0.5, 1.0, 0.0, 1.0,
	};

	unsigned int indices[] = {
		0,1,2,
		2,3,0,
		4,5,6,
		6,7,4,
	};
	//Mesh mesh1 = Mesh(vertices, indices, sizeof(vertices), sizeof(indices));
	//Mesh mesh1 = Mesh(vertices, indices, 40, 12);
	//mesh1.position.x = 1.2f;
	/*float vertices2[] = {
		 0.25, 1.0, 0.0,   1.0, 1.0, 1.0, 1.0f, 1.0f,
		 0.25, -0.5, 0.0,  0.0, 0.0, 0.0, 1.0f, 0.0f,
		 -0.25, -0.5, 0.0, 0.0, 0.0, 0.0, 0.0f, 0.0f,
		 -0.25, 1.0, 0.0,  1.0, 1.0, 1.0, 0.0f, 1.0f
	};*/
	/*float vertices2[] = {
		 -0.5, -0.5, 0.0, 1.0f, 1.0f,
		 0.5, -0.5, 0.0, 1.0f, 0.0f,
		 0.0, 0.5, 0.0, 0.0f, 0.0f
	};*/

	float vertices2[] = {
		 0.25, 1.0, 0.0, 1.0f, 1.0f,
		 0.25, -0.5, 0.0, 1.0f, 0.0f,
		 -0.25, -0.5, 0.0, 0.0f, 0.0f,
		 -0.25, 1.0, 0.0, 0.0f, 1.0f
	};
	unsigned int indices2[] = {
		0,1,2,
		2,3,0,
	};

	GameObject myGameObject1 = GameObject();
	GameObject myGameObject2 = GameObject();
	GameObject myGameObject3 = GameObject();
	//myGameObject.transform = 
	//Mesh mesh2 = Mesh(vertices2, indices2, sizeof(vertices2), sizeof(indices2));

	Mesh mesh4 = Mesh("ConeTriangulate.obj");
	Mesh mesh3 = Mesh("CubeTriangulate.obj");
	Mesh mesh5 = Mesh("CubeTriangulate.obj");

	myGameObject1.components.emplace_back(mesh3);
	mesh3.SetGameObject(&myGameObject1);

	myGameObject1.transform.position.x = 4;
	//myGameObject1.AddComponent<Mesh>();
	//AddComponent(Mesh());
	//AddComponent2<Mesh>();

	myGameObject2.components.emplace_back(mesh4);
	mesh4.SetGameObject(&myGameObject2);
	myGameObject2.transform.position.x = 0;

	myGameObject3.components.emplace_back(mesh5);
	mesh5.SetGameObject(&myGameObject3);
	myGameObject3.transform.position.x = 0;
	myGameObject3.transform.position.y = -2;

	Shader shader = Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader shader3 = Shader("vertexStandard.shader", "fragmentStandard.shader");
	Shader shader2 = Shader("vertex2.shader", "fragment2.shader");

	Shader shaderText = Shader("vertexText.shader", "fragmentText.shader");

	Texture texture1 = Texture("Brick.png", 0);

	Texture texture2 = Texture("Dry Dirt.png", 1);

	//Wood and metal 1
	Texture texture3 = Texture("container.png", 2);
	Texture texture4 = Texture("container_specular.png", 3);

	//Wood and metal 2
	Texture texture5 = Texture("Wood_Gate_Fortified_basecolor.jpg", 4);
	Texture texture6 = Texture("Wood_Gate_Fortified_metallic.jpg", 5);

	shader.SetShaderTexture("material.diffuse", texture3);
	shader.SetShaderTexture("material.specular", texture4);
	shader3.SetShaderTexture("material.diffuse", texture5);
	shader3.SetShaderTexture("material.specular", texture6);


	shader.Use();
	bool running = true;
	bool wireframe = false;
	//Set wireframe
	if (wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

	float animation = 0;
	int frameCount = 0;

	//mesh1.shader = &shader;
	//mesh2.shader = &shader2;
	mesh3.shader = &shader3;
	mesh4.shader = &shader3;
	mesh5.shader = &shader3;

	SDL_SetRelativeMouseMode(SDL_TRUE);

	GameObject pointLightGameObject = GameObject();
	PointLight pointLight = PointLight();
	pointLight.SetGameObject(&pointLightGameObject);
	pointLight.type = Light::Point;
	pointLight.gameObject->transform.position = Vector3(1.5f, 1.5, 1.5f);
	pointLight.color = Vector3(1, 0.1f, 0.1f);
	pointLight.intensity = 10;
	pointLight.SetRange(7);

	GameObject spotLightGameObject = GameObject();
	SpotLight spotLight = SpotLight();
	spotLight.SetGameObject(&spotLightGameObject);
	spotLight.type = Light::Spot;
	spotLight.gameObject->transform.position = Vector3(0, 3, 0);
	spotLight.gameObject->transform.rotation = Vector3(0.0f, -1.0f, 0.0f);
	spotLight.color = Vector3(0.05f, 0.05f, 1);
	spotLight.intensity = 200;
	spotLight.SetRange(7);
	spotLight.cutOff = glm::cos(glm::radians(12.0f));
	spotLight.outerCutOff = glm::cos(glm::radians(17.0f));

	GameObject directionalLightGameObject = GameObject();
	DirectionalLight directionalLight = DirectionalLight();
	directionalLight.SetGameObject(&directionalLightGameObject);
	directionalLight.type = Light::Directional;
	directionalLight.color = Vector3(0.3f, 0.7f, 0.3f);
	directionalLight.intensity = 1;
	directionalLight.gameObject->transform.rotation = Vector3(0.0f, -1.0f, -1.0f);

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
		GameLoop();

		if (InputSystem::GetKeyDown(ESCAPE))
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}

		if (InputSystem::GetKeyDown(A))
		{
			wireframe = !wireframe;
			//Set wireframe
			if (wireframe) {
				glLineWidth(2);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
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

		Graphics::DrawAllMesh();

		//UiManager::RenderText(shaderText, "#", 0.0f, 720.0f - 48, 1.0f, glm::vec3(0.5, 0.0f, 0.2f));
		//UiManager::RenderText(shaderText, "A", 0.0f, 720.0f - 55, 1.0f, glm::vec3(0.5, 0.0f, 0.2f));
		std::string wireframeText = std::string("Wireframe (A): ") + (wireframe ? "True" : "False");
		UiManager::RenderText(shaderText, wireframeText, 0.0f, 720.0f - 24, 0.5f, glm::vec3(0.5, 0.0f, 0.2f));
		Window::UpdateScreen();
		frameCount++;
	}

	glfwTerminate();
	return 0;
}

void GameLoop()
{

}