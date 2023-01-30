#include "game.h"
#include "audio/audio_clip.h"
#include "gameobject.h"
#include "graphics/graphics.h"
#include "graphics/mesh.h"
#include "graphics/camera.h"
#include <SDL2/SDL.h>
#include "lighting/lighting.h"

GameObject cameraGameObject = GameObject();
Camera camera = Camera();;

GameObject myGameObject1 = GameObject();
GameObject myGameObject2 = GameObject();
GameObject myGameObject3 = GameObject();

void Game::Init() {
	//loadedScenes.push_back(new Scene());
	//usedScene = loadedScenes[0];

	AudioClip audio1 = AudioClip("camera-13695.mp3");
	//AudioSource::Play2DSound(audio1);
	//AudioSource::Play3DSound(audio1, Vector3(0, 0, 2));

	GameObject cameraGameObject = GameObject();
	camera.SetGameObject(&cameraGameObject);
	camera.gameObject->transform.position = Vector3(0, 2, 2);

	Mesh mesh4 = Mesh("ConeTriangulate.obj");
	Mesh mesh3 = Mesh("CubeTriangulate.obj");
	Mesh mesh5 = Mesh("CubeTriangulate.obj");

	myGameObject1.components.push_back(mesh3);
	mesh3.SetGameObject(&myGameObject1);

	myGameObject1.transform.position.x = 4;

	myGameObject2.components.push_back(mesh4);
	mesh4.SetGameObject(&myGameObject2);
	myGameObject2.transform.position.x = 0;

	myGameObject3.components.push_back(mesh5);
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
}

void Game::Loop() 
{

}