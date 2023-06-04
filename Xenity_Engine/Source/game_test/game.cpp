#include "game.h"
#include "../xenity.h"

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
	GameObject *spriteGo2 = new GameObject();
	GameObject *spriteGo3 = new GameObject();
	spriteGo4 = new GameObject();
	GameObject *spriteGo5 = new GameObject();
	GameObject *spriteGo6 = new GameObject();

	spriteGo0->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo1->GetTransform()->SetPosition(Vector3(-2, 0, -2));
	// spriteGo1->GetTransform()->SetPosition(Vector3(2.56f, 2.56f, 0));
	spriteGo2->GetTransform()->SetPosition(Vector3(0, 0.81f, -0.36f));
	spriteGo3->GetTransform()->SetPosition(Vector3(4.56f, 2.56f, 0));
	spriteGo4->GetTransform()->SetPosition(Vector3(0, 0, 0));
	spriteGo5->GetTransform()->SetPosition(Vector3(1, 1, 0));
	spriteGo6->GetTransform()->SetPosition(Vector3(0, 0, 0));

	// spriteGo4->GetTransform()->SetRotation(Vector3(0, 90, 0));
	// spriteGo6->GetTransform()->SetRotation(Vector3(0, 90, 0));

	SpriteRenderer *ps0 = spriteGo0->AddComponent<SpriteRenderer>();
	SpriteRenderer *ps1 = spriteGo1->AddComponent<SpriteRenderer>();
	// SpriteRenderer *ps2 = spriteGo2->AddComponent<SpriteRenderer>();
	// SpriteRenderer *ps3 = spriteGo3->AddComponent<SpriteRenderer>();
	ps0->texture = texture5;
	ps1->texture = texture;
	// ps2->texture = texture4;
	// ps3->texture = TextManager::fonts[0]->Characters[65].texture;

	TextRenderer *tr = spriteGo4->AddComponent<TextRenderer>();
	tr->text = "Salut.\nComment ca va?q\nca va bien!\nOk!oooo";
	tr->horizontalAligment = H_Right;
	tr->verticalAlignment = V_Bottom;

	// TextRenderer *tr2 = spriteGo6->AddComponent<TextRenderer>();
	// tr2->text = "WWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWW";
	// tr2->horizontalAligment = H_Right;
	// tr2->verticalAlignment = V_Bottom;

	debugTextRenderer = spriteGo5->AddComponent<TextRendererCanvas>();
	// debugTextRenderer->horizontalAligment = H_Center;
	// debugTextRenderer->verticalAlignment = V_Center;
	debugTextRenderer->horizontalAligment = H_Left;
	debugTextRenderer->verticalAlignment = V_Top;
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

	// debugTextRenderer->text = "";
	for (const auto &kv : Performance::profilerList)
	{
		debugText += kv.first + " " + std::to_string(kv.second->GetValue()) + ", avg: " + std::to_string(kv.second->average) + "\n";
	}

	// debugTextRenderer->text = "WWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\nWWWWWWWWWWWWWWWWWWWWWW\n";
	debugTextRenderer->text = debugText;
	// debugTextRenderer->text = "ABC";
	if ((int)Time::GetTime() % 2 == 0 && (int)Time::GetTime() != lastTime)
	{
		lastTime = (int)Time::GetTime();
		Debug::Print(debugText);
	}
}