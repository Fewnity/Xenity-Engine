#include "game.h"
#include "../xenity.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/graphics/3d_graphics/mesh_data.h"

#include <string.h>

#ifdef __vita__

#endif

Game *Game::game;

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	game = this;
	LoadGameData();

	cameraGO = new GameObject("Camera");
	Camera *camera = cameraGO->AddComponent<Camera>();
	camera->SetNearClippingPlane(0.4f);
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

	cameraPivot = new GameObject();
	cameraPivot->GetTransform()->SetPosition(Vector3(0, 0, 0));

	cameraGO->GetTransform()->SetLocalPosition(Vector3(0, 0, -13));
	// cameraGO->GetTransform()->SetRotation(Vector3(20, 45, 45));

	GameObject *spriteGo0 = new GameObject();
	GameObject *spriteGo1 = new GameObject();
	spriteGo4 = new GameObject();
	GameObject *spriteGo5 = new GameObject();
	GameObject *spriteGo6 = new GameObject();

	spriteGo0->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo1->GetTransform()->SetPosition(Vector3(-5, 0, -2));
	spriteGo4->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo4->GetTransform()->SetLocalScale(Vector3(50, 50, -50));
	// spriteGo4->GetTransform()->SetRotation(Vector3(45, 45, 45));

	spriteGo5->GetTransform()->SetPosition(Vector3(1, 1, 0));
	spriteGo6->GetTransform()->SetPosition(Vector3(0, 2.5f, 0));
	spriteGo6->GetTransform()->SetLocalScale(Vector3(1, 1, 1));
	spriteGo0->GetTransform()->SetLocalScale(Vector3(1, 1, 1));

	// SpriteRenderer *ps0 = spriteGo0->AddComponent<SpriteRenderer>();
	// ps0->texture = texture;

	// SpriteRenderer *ps1 = spriteGo1->AddComponent<SpriteRenderer>();
	// ps1->texture = texture2;

	TextRenderer *tr = spriteGo6->AddComponent<TextRenderer>();
	tr->text = "Hello\nWorld!";
	tr->horizontalAligment = H_Right;
	tr->verticalAlignment = V_Bottom;
	// tr->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	debugTextRenderer = spriteGo5->AddComponent<TextRendererCanvas>();
	debugTextRenderer->horizontalAligment = H_Left;
	debugTextRenderer->verticalAlignment = V_Top;
	// debugTextRenderer->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	// MeshData *mesh = WavefrontLoader::LoadFromRawData("testcube.obj");
	// MeshData *mesh = WavefrontLoader::LoadFromRawData("DonutTriangulate.obj");
	MeshData *mesh = WavefrontLoader::LoadFromRawData("DustPartsTest256.obj");
	MeshRenderer *meshRenderer = spriteGo4->AddComponent<MeshRenderer>();
	meshRenderer->meshData = mesh;
	meshRenderer->texture = texture;
	mesh->unifiedColor = Color::CreateFromRGBA(255, 218, 208, 255);

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
	Vector3 rot = cameraGO->GetTransform()->GetRotation();
	// Vector3 rot = cameraPivot->GetTransform()->GetRotation();
	Vector3 pos = cameraGO->GetTransform()->GetPosition();
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

	pos -= cameraGO->GetTransform()->GetForward() * (InputSystem::leftJoystick.y / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO->GetTransform()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f) * Time::GetDeltaTime() * 30;
#else
	// Rotate camera
	rot.x += InputSystem::rightJoystick.y * 1.5f * Time::GetDeltaTime() * 50;
	rot.y += InputSystem::rightJoystick.x * 1.5f * Time::GetDeltaTime() * 50;

	// Move camera
	pos -= cameraGO->GetTransform()->GetForward() * (InputSystem::leftJoystick.y / 7.0f) * Time::GetDeltaTime() * 30;
	pos -= cameraGO->GetTransform()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f) * Time::GetDeltaTime() * 30;
#endif

	if (InputSystem::GetKey(UP))
		pos.y -= (-1 / 7.0f) * Time::GetDeltaTime() * 30;
	else if (InputSystem::GetKey(DOWN))
		pos.y -= (1 / 7.0f) * Time::GetDeltaTime() * 30;

	cameraGO->GetTransform()->SetPosition(pos);
	// cameraGO->GetTransform()->SetRotation(Vector3::LookAt(pos, Vector3(0, 0, 0)));
	cameraGO->GetTransform()->SetRotation(rot);
	// cameraPivot->GetTransform()->SetRotation(rot);

	std::string fpsText = std::to_string(1.0f / Time::GetUnscaledDeltaTime());
	std::string debugText = "FPS: " + fpsText.substr(0, fpsText.size() - 4) + "\n";
	debugText += "DrawCall " + std::to_string(Performance::GetDrawCallCount()) + "\n";
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

	debugTextRenderer->text = debugText;
	// if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
	// {
	// 	lastTime = (int)Time::GetTime();
	// 	Debug::Print(debugText);
	// }

	// Vector3 meshRot = spriteGo4->GetTransform()->GetRotation();
	// meshRot += spriteGo4->GetTransform()->GetLeft() * 2 + spriteGo4->GetTransform()->GetForward() * 3;
	// spriteGo4->GetTransform()->SetRotation(meshRot);
}