#include "game.h"
#include "../xenity.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/graphics/3d_graphics/mesh_data.h"

#ifdef __vita__
#include <psp2/touch.h>
#endif

Game *Game::game;

// SceTouchData touch[SCE_TOUCH_PORT_MAX_NUM];

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	game = this;
	LoadGameData();

	// sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, SCE_TOUCH_SAMPLING_STATE_START);
	// sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, SCE_TOUCH_SAMPLING_STATE_START);
	// sceTouchEnableTouchForce(SCE_TOUCH_PORT_FRONT);
	// sceTouchEnableTouchForce(SCE_TOUCH_PORT_BACK);

	Texture *texture = new Texture("container.jpg", "Container");
	texture->SetFilter(Texture::Point);
	texture->SetWrapMode(Texture::ClampToEdge);
	Texture *texture5 = new Texture("dot.jpg", "Dot");

	cameraGO = GameObject::FindGameObjectByName("Camera");
	cameraGO->GetTransform()->SetPosition(Vector3(0, 0, -5));
	cameraGO->GetTransform()->SetRotation(Vector3(0, 0, 0));

	GameObject *spriteGo0 = new GameObject();
	GameObject *spriteGo1 = new GameObject();
	spriteGo4 = new GameObject();
	GameObject *spriteGo5 = new GameObject();
	GameObject *spriteGo6 = new GameObject();

	spriteGo0->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo1->GetTransform()->SetPosition(Vector3(-2, 0, -2));
	spriteGo4->GetTransform()->SetPosition(Vector3(0, 0, 0));
	// spriteGo4->GetTransform()->SetRotation(Vector3(45, 45, 45));

	spriteGo5->GetTransform()->SetPosition(Vector3(1, 1, 0));
	spriteGo6->GetTransform()->SetPosition(Vector3(0, 2.5f, 0));

	SpriteRenderer *ps0 = spriteGo0->AddComponent<SpriteRenderer>();
	SpriteRenderer *ps1 = spriteGo1->AddComponent<SpriteRenderer>();
	ps0->texture = texture5;
	ps1->texture = texture;

	TextRenderer *tr = spriteGo6->AddComponent<TextRenderer>();
	tr->text = "Hello\nWorld!";
	tr->horizontalAligment = H_Right;
	tr->verticalAlignment = V_Bottom;

	debugTextRenderer = spriteGo5->AddComponent<TextRendererCanvas>();
	debugTextRenderer->horizontalAligment = H_Left;
	debugTextRenderer->verticalAlignment = V_Top;

	// MeshData *mesh = WavefrontLoader::LoadFromRawData("testcube.obj");
	MeshData *mesh = WavefrontLoader::LoadFromRawData("DonutTriangulate.obj");
	MeshRenderer *meshRenderer = spriteGo4->AddComponent<MeshRenderer>();
	meshRenderer->meshData = mesh;
	meshRenderer->texture = texture;
}

void Game::LoadGameData()
{
}

int lastTime = 0;

/// <summary>
/// Game loop
/// </summary>
void Game::Update()
{
	Vector3 rot = cameraGO->GetTransform()->GetRotation();
	Vector3 pos = cameraGO->GetTransform()->GetPosition();
#ifdef __PSP__
	// Rotate camera
	if (InputSystem::GetKey(TRIANGLE))
		rot.x += -1.5f;
	else if (InputSystem::GetKey(CROSS))
		rot.x += 1.5f;

	if (InputSystem::GetKey(CIRCLE))
		rot.y += 1.5f;
	else if (InputSystem::GetKey(SQUARE))
		rot.y += -1.5f;

	pos -= cameraGO->GetTransform()->GetForward() * (InputSystem::leftJoystick.y / 7.0f);
	pos -= cameraGO->GetTransform()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f);
#else
	// Rotate camera
	rot.x += InputSystem::rightJoystick.y * 1.5f;
	rot.y += InputSystem::rightJoystick.x * 1.5f;

	// Move camera
	pos -= cameraGO->GetTransform()->GetForward() * (InputSystem::leftJoystick.y / 7.0f);
	pos -= cameraGO->GetTransform()->GetLeft() * (InputSystem::leftJoystick.x / 7.0f);
#endif

	if (InputSystem::GetKey(UP))
		pos.y -= (-1 / 7.0f);
	else if (InputSystem::GetKey(DOWN))
		pos.y -= (1 / 7.0f);

	cameraGO->GetTransform()->SetPosition(pos);
	cameraGO->GetTransform()->SetRotation(rot);

	std::string fpsText = std::to_string(1.0f / Time::GetUnscaledDeltaTime());
	std::string debugText = "FPS: " + fpsText.substr(0, fpsText.size() - 4) + "\n";
	debugText += "DrawCall " + std::to_string(Performance::GetDrawCallCount()) + "\n";
	// for (int screen = 0; screen < SCE_TOUCH_PORT_MAX_NUM; screen++)
	// {
	// 	sceTouchPeek(screen, &touch[screen], 1);
	// 	debugText += "screen: " + std::to_string(screen) + "\n";
	// 	for (int finger = 0; finger < SCE_TOUCH_MAX_REPORT; finger++)
	// 	{
	// 		debugText += "finger: " + std::to_string(finger) + ", " + std::to_string(touch[screen].report[finger].x) + ", " + std::to_string(touch[screen].report[finger].y) + "\n";
	// 	}
	// 	/* code */
	// }

	for (const auto &kv : Performance::profilerList)
	{
		debugText += kv.first + " " + std::to_string(kv.second->GetValue()) + ", avg: " + std::to_string(kv.second->average) + "\n";
	}

	debugTextRenderer->text = debugText;
	if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
	{
		lastTime = (int)Time::GetTime();
		Debug::Print(debugText);
	}

	Vector3 meshRot = spriteGo4->GetTransform()->GetRotation();
	meshRot += spriteGo4->GetTransform()->GetLeft() * 2 + spriteGo4->GetTransform()->GetForward() * 3;
	spriteGo4->GetTransform()->SetRotation(meshRot);
}