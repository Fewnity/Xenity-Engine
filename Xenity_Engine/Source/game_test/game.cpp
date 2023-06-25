#include "game.h"
#include "../xenity.h"
#include "../engine/graphics/3d_graphics/mesh_data.h"
#include "../engine/graphics/renderer/renderer.h"
#include <string.h>
#include <thread>
#include <malloc.h>

#ifdef __PSP__
#include <pspkernel.h>
#include <psppower.h>
#endif

#ifdef __vita__
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/power.h>
#endif

Game *Game::game;

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	game = this;
	LoadGameData();

	cameraGO = CreateGameObject("Camera");
	auto camera = cameraGO.lock()->AddComponent<Camera>().lock();
	camera->SetNearClippingPlane(0.2f);
	camera->SetFarClippingPlane(30);
	camera->SetProjectionSize(5.0f);
	camera->SetFov(70);
	// camera->SetProjectionType(Orthographic);

	// Texture *texture = new Texture("container.jpg", "Container");
	Texture *texture = new Texture("Atlas.bmp", "Atlas", Texture::Point, true, true);
	// Texture *texture2 = new Texture("blueStructure_2.png", "blueStructure_2", Texture::Point, false, true);

	// Texture *texture = new Texture("Dry Dirt_low.png", "Dry Dirt");
	// texture->SetFilter(Texture::Point);
	// // texture->SetWrapMode(Texture::ClampToEdge);
	// texture->SetWrapMode(Texture::Repeat);

	// Texture *texture5 = new Texture("dot.jpg", "Dot");

	cameraPivot = CreateGameObject();
	cameraPivot.lock()->GetTransform().lock()->SetPosition(Vector3(0, 0, 0));

	// return;
	cameraGO.lock()->GetTransform().lock()->SetLocalPosition(Vector3(0, 0, -13));
	// cameraGO->GetTransform()->SetRotation(Vector3(20, 45, 45));

	std::weak_ptr<GameObject> spriteGo0 = CreateGameObject();
	std::weak_ptr<GameObject> spriteGo1 = CreateGameObject();
	spriteGo4 = CreateGameObject();
	std::weak_ptr<GameObject> spriteGo5 = CreateGameObject();
	std::weak_ptr<GameObject> spriteGo6 = CreateGameObject();

	spriteGo0.lock()->GetTransform().lock()->SetPosition(Vector3(0, 0, 0));
	spriteGo1.lock()->GetTransform().lock()->SetPosition(Vector3(-5, 0, -2));
	spriteGo4.lock()->GetTransform().lock()->SetPosition(Vector3(0, 0, 0));
	spriteGo4.lock()->GetTransform().lock()->SetLocalScale(Vector3(50, 50, -50));
	// spriteGo4->GetTransform()->SetRotation(Vector3(45, 45, 45));

	spriteGo5.lock()->GetTransform().lock()->SetPosition(Vector3(1, 1, 0));
	spriteGo6.lock()->GetTransform().lock()->SetPosition(Vector3(0, 2.5f, 0));
	spriteGo6.lock()->GetTransform().lock()->SetLocalScale(Vector3(1, 1, 1));
	spriteGo0.lock()->GetTransform().lock()->SetLocalScale(Vector3(1, 1, 1));

	// SpriteRenderer *ps0 = spriteGo0->AddComponent<SpriteRenderer>();
	// ps0->texture = texture;

	// SpriteRenderer *ps1 = spriteGo1->AddComponent<SpriteRenderer>();
	// ps1->texture = texture2;

	auto tr = spriteGo6.lock()->AddComponent<TextRenderer>().lock();
	tr->text = "Hello\nWorld!";
	tr->horizontalAligment = H_Right;
	tr->verticalAlignment = V_Bottom;
	// tr->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	debugTextRenderer = spriteGo5.lock()->AddComponent<TextRendererCanvas>();
	debugTextRenderer.lock()->horizontalAligment = H_Left;
	debugTextRenderer.lock()->verticalAlignment = V_Top;
	// debugTextRenderer->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	// MeshData *mesh = MeshManager::LoadMesh("testcube.obj");
	// MeshData *mesh = MeshManager::LoadMesh("DonutTriangulate.obj");
	MeshData *mesh = MeshManager::LoadMesh("DustPartsTest256.obj");
	auto meshRenderer = spriteGo4.lock()->AddComponent<MeshRenderer>().lock();
	meshRenderer->meshData = mesh;
	meshRenderer->texture = texture;
	// mesh->unifiedColor = Color::CreateFromRGBA(255, 218, 208, 255);
	mesh->unifiedColor = Color::CreateFromRGBA(255, 255, 255, 255);

	// Destroy(meshRenderer);
	// spriteGo4.lock()->SetActive(false);
	// Destroy(spriteGo4);

	// meshRenderer = spriteGo1->AddComponent<MeshRenderer>();
	// meshRenderer->meshData = mesh;
	// meshRenderer->texture = texture;

	// Texture *tile0 = new Texture("scifiTile_16_low2.png", "scifiTile_16");
	// Texture *tile0 = new Texture("scifiTile_16.png", "scifiTile_16");
	// // Texture *tile0 = new Texture("scifiTile_16_low2.png", "scifiTile_16");
	// // tile0->SetFilter(Texture::Point);
	// // Texture *tile1 = new Texture("scifiTile_30.png", "scifiTile_30");
	// // Texture *tile2 = new Texture("scifiTile_41.png", "scifiTile_41");

	// tilemapGO = new GameObject();
	// tilemap = tilemapGO->AddComponent<Tilemap>();
	// int tilemapSize = 100;
	// tilemap->Setup(tilemapSize, tilemapSize);
	// tilemap->AddTexture(tile0);
	// // tilemap->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	// for (int x = 0; x < tilemapSize; x++)
	// {
	// 	for (int y = 0; y < tilemapSize; y++)
	// 	{
	// 		tilemap->SetTile(x, y, 1);
	// 	}
	// }

	InputSystem::HideMouse();

	std::weak_ptr<GameObject> gmLight0 = CreateGameObject();
	auto light0 = gmLight0.lock()->AddComponent<Light>().lock();
	// light0->SetupPointLight(Color::CreateFromRGBAFloat(1, 0,0,0), 1, 10);
	// light0->SetupDirectionalLight(Color::CreateFromRGBA(255, 218, 208, 1), 1);
	light0->SetupDirectionalLight(Color::CreateFromRGBA(130, 70, 70, 1), 1);
	light0->GetTransform().lock()->SetPosition(Vector3(0, 0, 0));
	light0->GetTransform().lock()->SetRotation(Vector3(0, 0, 0));

	std::weak_ptr<GameObject> gmLight1 = CreateGameObject();
	auto light1 = gmLight1.lock()->AddComponent<Light>().lock();
	light1->SetupPointLight(Color::CreateFromRGBAFloat(1, 0, 0, 1), 1, 10);
	light1->GetTransform().lock()->SetPosition(Vector3(0, 0, -2));

	AudioClip *audioClip1 = new AudioClip("Special_Needs.wav");
	std::weak_ptr<GameObject> gmAudio1 = CreateGameObject();
	myAudioSource = gmAudio1.lock()->AddComponent<AudioSource>().lock();
	myAudioSource.lock()->audioClip = audioClip1;
	myAudioSource.lock()->Play();

	AudioClip *audioClip0 = new AudioClip("Wind.wav");
	// AudioClip *audioClip0 = new AudioClip("Special_Needs_44100.mp3");
	std::weak_ptr<GameObject> gmAudio0 = CreateGameObject("gmAudio0");
	if (auto gmAudio0Lock = gmAudio0.lock())
	{
		auto audioSource0 = gmAudio0Lock->AddComponent<AudioSource>().lock();
		if (audioSource0)
		{
			audioSource0->audioClip = audioClip0;
			audioSource0->Play();
		}
	}
}

void Game::LoadGameData()
{
}

int lastTime = 0;

// void RotateAround(Transform *trans, Vector3 targetPosition, Vector3 axis, float angle)
// {
// 	// axis.x = -axis.x;
// 	// axis.z = -axis.z;
// 	// Vector3 targetPosition = Vector3(0,0,0);
// 	// Convertir l'angle en radians
// 	float radAngle = angle * (3.14159f / 180.0f);

// 	// Calculer la position du vecteur à tourner par rapport à la cible
// 	Vector3 direction = trans->GetPosition() - targetPosition;

// 	// Calculer la matrice de rotation
// 	float cosTheta = cos(radAngle);
// 	float sinTheta = sin(radAngle);

// 	float x = direction.x;
// 	float y = direction.y;
// 	float z = direction.z;
// 	float xPrime = (cosTheta + (1 - cosTheta) * axis.x * axis.x) * x +
// 				   ((1 - cosTheta) * axis.x * axis.y - axis.z * sinTheta) * y +
// 				   ((1 - cosTheta) * axis.x * axis.z + axis.y * sinTheta) * z;

// 	float yPrime = ((1 - cosTheta) * axis.x * axis.y + axis.z * sinTheta) * x +
// 				   (cosTheta + (1 - cosTheta) * axis.y * axis.y) * y +
// 				   ((1 - cosTheta) * axis.y * axis.z - axis.x * sinTheta) * z;

// 	float zPrime = ((1 - cosTheta) * axis.x * axis.z - axis.y * sinTheta) * x +
// 				   ((1 - cosTheta) * axis.y * axis.z + axis.x * sinTheta) * y +
// 				   (cosTheta + (1 - cosTheta) * axis.z * axis.z) * z;

// 	// axis.y = -axis.y;
// 	trans->SetPosition(targetPosition + Vector3(xPrime, yPrime, zPrime));
// 	// trans->SetRotation(trans->GetRotation() + axis * angle);

// 	// Mettre à jour la position du vecteur après rotation
// 	// return targetPosition + Vector3(xPrime, yPrime, zPrime);
// }

/// <summary>
/// Game loop
/// </summary>
void Game::Update()
{
	if (InputSystem::GetKeyDown(CROSS))
		myAudioSource.lock()->Play();
	else if (InputSystem::GetKeyDown(CIRCLE))
		myAudioSource.lock()->Pause();
	else if (InputSystem::GetKeyDown(TRIANGLE))
		Destroy(myAudioSource);

	// return;
	Vector3 rot = cameraGO.lock()->GetTransform().lock()->GetRotation();
	// Vector3 rot = cameraPivot->GetTransform()->GetRotation();
	Vector3 pos = cameraGO.lock()->GetTransform().lock()->GetPosition();
	// Engine::renderer->SetLight(pos);
#ifdef __PSP__
	// rot += -InputSystem::leftJoystick.x * cameraPivot->GetTransform()->GetUp() + InputSystem::leftJoystick.y * cameraPivot->GetTransform()->GetLeft();

	// rot += -InputSystem::leftJoystick.x * cameraPivot->GetTransform()->GetForward() + -InputSystem::leftJoystick.y * Vector3(1, 0, 0);

	// rot += -InputSystem::leftJoystick.x * cameraPivot->GetTransform()->GetUp() + -InputSystem::leftJoystick.y * Vector3(1, 0, 0);

	// RotateAround(cameraGO->GetTransform(), Vector3(0, 0, 0), cameraGO->GetTransform()->GetUp(), -InputSystem::leftJoystick.x * Time::GetDeltaTime() * 10);
	//  pos = RotateAround(pos, Vector3(0, 0, 0), Vector3(0, 0, 1), -InputSystem::leftJoystick.y);
	// RotateAround(cameraGO->GetTransform(), Vector3(0, 0, 0), cameraGO->GetTransform()->GetRight(), -InputSystem::leftJoystick.y * Time::GetDeltaTime() * 10);

	// Rotate camera
	if (InputSystem::GetKey(TRIANGLE))
		rot.x += -1.5f * Time::GetDeltaTime() * 50;
	else if (InputSystem::GetKey(CROSS))
		rot.x += 1.5f * Time::GetDeltaTime() * 50;

	if (InputSystem::GetKey(CIRCLE))
		rot.y += 1.5f * Time::GetDeltaTime() * 50;
	else if (InputSystem::GetKey(SQUARE))
		rot.y += -1.5f * Time::GetDeltaTime() * 50;

	pos -= cameraGO.lock()->GetTransform().lock()->GetForward() * (InputSystem::leftJoystick.y / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO.lock()->GetTransform().lock()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f) * Time::GetDeltaTime() * 30;
#elif defined(__vita__)
	// Rotate camera
	rot.x += InputSystem::rightJoystick.y * 1.5f * Time::GetDeltaTime() * 50;
	rot.y += InputSystem::rightJoystick.x * 1.5f * Time::GetDeltaTime() * 50;

	// Move camera
	pos -= cameraGO.lock()->GetTransform().lock()->GetForward() * (InputSystem::leftJoystick.y / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO.lock()->GetTransform().lock()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f) * Time::GetDeltaTime() * 30;
#elif defined(_WIN32) || defined(_WIN64)

	rot.x += -InputSystem::mouseSpeed.y * Time::GetDeltaTime() * 10000;
	rot.y += InputSystem::mouseSpeed.x * Time::GetDeltaTime() * 10000;

	float fwd = 0;
	float side = 0;
	if (InputSystem::GetKey(UP))
		fwd = -1;
	else if (InputSystem::GetKey(DOWN))
		fwd = 1;

	if (InputSystem::GetKey(RIGHT))
		side = 1;
	else if (InputSystem::GetKey(LEFT))
		side = -1;

	pos -= cameraGO.lock()->GetTransform().lock()->GetForward() * (fwd / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO.lock()->GetTransform().lock()->GetLeft() * (side / 7.0f) * Time::GetDeltaTime() * 30;
#endif

	/*if (InputSystem::GetKey(UP))
		pos.y -= (-1 / 7.0f) * Time::GetDeltaTime() * 30;
	else if (InputSystem::GetKey(DOWN))
		pos.y -= (1 / 7.0f) * Time::GetDeltaTime() * 30;*/

	cameraGO.lock()->GetTransform().lock()->SetPosition(pos);
	// cameraGO->GetTransform()->SetRotation(Vector3::LookAt(pos, Vector3(0, 0, 0)));
	cameraGO.lock()->GetTransform().lock()->SetRotation(rot);
	// cameraPivot->GetTransform()->SetRotation(rot);

	std::string fpsText = std::to_string(1.0f / Time::GetUnscaledDeltaTime());
	std::string debugText = "FPS: " + fpsText.substr(0, fpsText.size() - 4) + "\n";
	debugText += "DrawCall " + std::to_string(Performance::GetDrawCallCount()) + "\n";
	debugText += "x " + std::to_string(pos.x) + " y " + std::to_string(pos.y) + " z " + std::to_string(pos.z) + "\n";
#if defined(__PSP__)
	debugText += "clock: CPU " + std::to_string(scePowerGetCpuClockFrequencyInt()) + ", BUS " + std::to_string(scePowerGetBusClockFrequencyInt());
#elif defined(__vita__)
	debugText += "clock: ARM " + std::to_string(scePowerGetArmClockFrequency()) + ", BUS " + std::to_string(scePowerGetBusClockFrequency()) + ", GPU " + std::to_string(scePowerGetGpuClockFrequency()) + ", GPU Xbar " + std::to_string(scePowerGetGpuXbarClockFrequency());
#endif
	// debugText += "pos: " + std::to_string(pos.x) + " " + std::to_string(pos.y) + " " + std::to_string(pos.z) + "\n";
	// debugText += "rot: " + std::to_string(rot.x) + " " + std::to_string(rot.y) + " " + std::to_string(rot.z) + "\n";
	// debugText += "pos: " + std::to_string(cameraGO->GetTransform()->GetPosition().x) + " " + std::to_string(cameraGO->GetTransform()->GetPosition().y) + " " + std::to_string(cameraGO->GetTransform()->GetPosition().z) + "\n";

	// for (int i = 0; i < InputSystem::GetTouchScreenCount(); i++)
	// {
	// 	int touchCount = InputSystem::GetTouchCount(i);
	// 	debugText += "screen " + std::to_string(i) + " touch count: " + std::to_string(touchCount) + "\n";
	// 	for (int touchI = 0; touchI < touchCount; touchI++)
	// 	{
	// 		Touch touch = InputSystem::GetTouch(touchI, i);
	// 		debugText += "Touch " + std::to_string(touchI) + " x: " + std::to_string(touch.position.x) + " y: " + std::to_string(touch.position.y) + " id: " + std::to_string(touch.fingerId) + "\n";
	// 	}
	// }

	// for (const auto &kv : Performance::profilerList)
	// {
	// 	debugText += kv.first + " " + std::to_string(kv.second->GetValue()) + ", avg: " + std::to_string(kv.second->average) + "\n";
	// }

	// debugTextRenderer.lock()->text = Debug::debugText;
	debugTextRenderer.lock()->text = debugText;

	//  if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
	//  {
	//  	lastTime = (int)Time::GetTime();
	//  	Debug::Print(debugText);
	//  }

	// Vector3 meshRot = spriteGo4->GetTransform()->GetRotation();
	// meshRot += spriteGo4->GetTransform()->GetLeft() * 2 + spriteGo4->GetTransform()->GetForward() * 3;
	// spriteGo4->GetTransform()->SetRotation(meshRot);
}