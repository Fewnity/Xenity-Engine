#include "game.h"
#include "../xenity.h"
#include "../engine/file_system/mesh_loader/wavefront_loader.h"
#include "../engine/graphics/3d_graphics/mesh_data.h"

#include <string.h>
#include <thread>

#ifdef __PSP__
#include <pspaudiolib.h>
#include <pspaudio.h>
#endif

#ifdef __vita__
#include <psp2/audioout.h>
#include <psp2/kernel/threadmgr.h>
#endif

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"
#define DR_MP3_IMPLEMENTATION
#include "dr_mp3.h"

Game *Game::game;

/*drmp3 mp3;
drmp3_int16 *pDecodedInterleavedPCMFrames;
// drwav_int16 *pDecodedInterleavedPCMFrames;
int seekPosition = 0;
int size = 2048;
// int size = 2048 * 2;
int freq = 7;
int mode = SCE_AUDIO_OUT_MODE_STEREO;
int vol = SCE_AUDIO_VOLUME_0DB;
int port;*/

typedef struct
{
	short l, r;
} sample_t;

void audioCallback(void *buf, unsigned int length, void *userdata)
{
	sample_t *ubuf = (sample_t *)buf;

	// drmp3_int16 *pDecodedInterleavedPCMFrames = (drwav_int16 *)malloc(length * mp3.channels * sizeof(drmp3_int16));
	// drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, length, pDecodedInterleavedPCMFrames);

	for (int i = 0; i < length; i++)
	{
		/*ubuf[i].l = pDecodedInterleavedPCMFrames[seekPosition];
		seekPosition++;
		ubuf[i].r = pDecodedInterleavedPCMFrames[seekPosition];
		seekPosition++;*/

		// ubuf[i].l = pDecodedInterleavedPCMFrames[0 + i * 2];
		// seekPosition++;
		// ubuf[i].r = pDecodedInterleavedPCMFrames[1 + i * 2];
		// seekPosition++;

		// ubuf[i].l = (rand() % 65536) / 10;
		// ubuf[i].r = (rand() % 65536) / 10;
	}
	// free(pDecodedInterleavedPCMFrames);
}

// int audio_thread(SceSize args, void *argp)
// {
// 	while (true)
// 	{
// 		/* code */
// 		if (sceAudioOutGetRestSample(port) == 0)
// 		{
// 			int16_t wave_buf[SCE_AUDIO_MAX_LEN] = {0};

// 			drmp3_int16 *pDecodedInterleavedPCMFrames2 = (drwav_int16 *)malloc(size * mp3.channels * sizeof(drmp3_int16));
// 			drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, size, pDecodedInterleavedPCMFrames2);
// 			seekPosition = 0;

// 			for (int i = 0; i < size; i++)
// 			{
// 				wave_buf[i * 2] = pDecodedInterleavedPCMFrames2[seekPosition];
// 				seekPosition++;
// 				wave_buf[1 + i * 2] = pDecodedInterleavedPCMFrames2[seekPosition];
// 				seekPosition++;
// 				// wave_buf[i] = pDecodedInterleavedPCMFrames[]
// 				// wave_buf[i] = rand() % 65536;
// 			}
// 			sceAudioOutOutput(port, wave_buf);
// 			free(pDecodedInterleavedPCMFrames2);
// 		}
// 	}
// }

/// <summary>
/// Init game
/// </summary>
void Game::Start()
{
	game = this;
	LoadGameData();

	cameraGO = CreateGameObject("Camera");
	Camera *camera = cameraGO.lock()->AddComponent<Camera>();
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

	cameraPivot = CreateGameObject();
	cameraPivot.lock()->GetTransform().lock()->SetPosition(Vector3(0, 0, 0));

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

	TextRenderer *tr = spriteGo6.lock()->AddComponent<TextRenderer>();
	tr->text = "Hello\nWorld!";
	tr->horizontalAligment = H_Right;
	tr->verticalAlignment = V_Bottom;
	// tr->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	debugTextRenderer = spriteGo5.lock()->AddComponent<TextRendererCanvas>();
	debugTextRenderer->horizontalAligment = H_Left;
	debugTextRenderer->verticalAlignment = V_Top;
	// debugTextRenderer->SetColor(Color::CreateFromRGBA(255, 255, 255, 255));

	// MeshData *mesh = WavefrontLoader::LoadFromRawData("testcube.obj");
	// MeshData *mesh = WavefrontLoader::LoadFromRawData("DonutTriangulate.obj");
	MeshData *mesh = WavefrontLoader::LoadFromRawData("DustPartsTest256.obj");
	MeshRenderer *meshRenderer = spriteGo4.lock()->AddComponent<MeshRenderer>();
	meshRenderer->meshData = mesh;
	meshRenderer->texture = texture;
	mesh->unifiedColor = Color::CreateFromRGBA(255, 218, 208, 255);

	//spriteGo4.lock()->SetActive(false);

	//Destroy(spriteGo4);

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

	/*int freqs[] = {8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
	port = sceAudioOutOpenPort(SCE_AUDIO_OUT_PORT_TYPE_BGM, size, freqs[freq], (SceAudioOutMode)mode);
	int volA[2] = {vol, vol};
	sceAudioOutSetVolume(port, (SceAudioOutChannelFlag)(SCE_AUDIO_VOLUME_FLAG_L_CH | SCE_AUDIO_VOLUME_FLAG_R_CH), volA);*/

	// drwav wav;
	// if (!drwav_init_file(&wav, "Special_Needs.wav", NULL))
	// {
	// 	Debug::Print("AUDIO ERROR");
	// 	// Error opening WAV file.
	// }
	// pDecodedInterleavedPCMFrames = (drwav_int16 *)malloc(wav.totalPCMFrameCount * wav.channels * sizeof(drwav_int16));
	// size_t numberOfSamplesActuallyDecoded = drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, pDecodedInterleavedPCMFrames);

	// MP3
	/*if (!drmp3_init_file(&mp3, "ux0:Special_Needs_low.mp3", NULL))
	{
		Debug::Print("AUDIO ERROR");
		// Failed to open file
	}*/
	// int frameCount = drmp3_get_pcm_frame_count(&mp3) / 10;
	// pDecodedInterleavedPCMFrames = (drmp3_int16 *)malloc(frameCount * mp3.channels * sizeof(drmp3_int16));
	// if (!pDecodedInterleavedPCMFrames)
	// {
	// 	Debug::Print("AUDIO NO MEME");
	// }
	// else
	// {
	// 	Debug::Print("AUDIO MEM");
	// 	drmp3_uint64 framesRead = drmp3_read_pcm_frames_s16(&mp3, frameCount, pDecodedInterleavedPCMFrames);
	// }

	// std::thread t(audio_thread);
	// t.join();

	// SceUID thd_id = sceKernelCreateThread("audio_thread", audio_thread, 0x40, 0x400000, 0, 0, NULL);
	// if (thd_id >= 0)
	// 	sceKernelStartThread(thd_id, 0, NULL);
	// pspAudioInit();
	// pspAudioSetChannelCallback(0, audioCallback, NULL);

	// drwav_uninit(&wav);
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
	Vector3 rot = cameraGO.lock()->GetTransform().lock()->GetRotation();
	// Vector3 rot = cameraPivot->GetTransform()->GetRotation();
	Vector3 pos = cameraGO.lock()->GetTransform().lock()->GetPosition();
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

	rot.x += -InputSystem::mouseSpeed.y * Time::GetDeltaTime() * 5000;
	rot.y += InputSystem::mouseSpeed.x * Time::GetDeltaTime() * 5000;

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

	if (InputSystem::GetKey(UP))
		debugText += "UP";
	if (InputSystem::GetKey(DOWN))
		debugText += "DOWN";
	if (InputSystem::GetKey(LEFT))
		debugText += "LEFT";
	if (InputSystem::GetKey(RIGHT))
		debugText += "RIGHT";

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