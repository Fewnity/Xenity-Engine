#include "rendering_test_2d.h"
#include "../xenity.h"

void RenderingTest2D::Init()
{
	LoadGameData();
	camera = cameraGameObject->AddComponent<Camera>();
	//cameraGameObject->AddExistingComponent(camera);
	camera->GetGameObject()->transform.SetPosition(Vector3(0, 0, -10));

	SpriteRenderer* gradientSprR = gradientGameObject->AddComponent<SpriteRenderer>();
	gradientSprR->texture = gradient;
	gradientSprR->material = material2D;
	//SpriteRenderer* gradientSprR = new SpriteRenderer(gradient, material2D);
	gradientSprR->color = Color::CreateFromRGBAFloat(1, 1, 1, 1.0f);
	//gradientGameObject->AddExistingComponent(gradientSprR);
	
	SpriteRenderer* tileSprR = tileGameObject->AddComponent<SpriteRenderer>();
	tileSprR->texture = textureTile0;
	tileSprR->material = material2D;
	//SpriteRenderer* tileSprR = new SpriteRenderer(textureTile0, material2D);
	tileSprR->color = Color::CreateFromRGBAFloat(0.5, 1, 1, 1.0f);
	//tileGameObject->AddExistingComponent(tileSprR);

	//Static tiles
	//SpriteRenderer* tileSprRB = new SpriteRenderer(textureTile0, material2D);
	SpriteRenderer* tileSprRB = tileGameObjectBack->AddComponent<SpriteRenderer>();
	tileSprRB->texture = textureTile0;
	tileSprRB->material = material2D;
	tileSprRB->color = Color::CreateFromRGBAFloat(1, 1, 1, 1.0f);
	tileSprRB->SetOrderInLayer(-1);
	//tileGameObjectBack->AddExistingComponent(tileSprRB);
	tileGameObjectBack->transform.SetPosition(Vector3(-2, 0, 0));

	//SpriteRenderer* tileSprRF = new SpriteRenderer(textureTile0, material2D);
	SpriteRenderer* tileSprRF = tileGameObjectFront->AddComponent<SpriteRenderer>();
	tileSprRF->texture = textureTile0;
	tileSprRF->material = material2D;
	tileSprRF->color = Color::CreateFromRGBAFloat(1, 1, 1, 1.0f);
	tileSprRF->SetOrderInLayer(1);
	//tileGameObjectFront->AddExistingComponent(tileSprRF);
	tileGameObjectFront->transform.SetPosition(Vector3(2, 0, 0));

	//Static gradients
	//SpriteRenderer* gradientSprRB = new SpriteRenderer(gradient, material2D);
	SpriteRenderer* gradientSprRB = gradientGameObjectBack->AddComponent<SpriteRenderer>();
	gradientSprRB->texture = gradient;
	gradientSprRB->material = material2D;
	gradientSprRB->color = Color::CreateFromRGBAFloat(1, 1, 1, 1.0f);
	//gradientGameObjectBack->AddExistingComponent(gradientSprRB);
	gradientGameObjectBack->transform.SetPosition(Vector3(-2, 1, -1));

	//SpriteRenderer* gradientSprRF = new SpriteRenderer(gradient, material2D);
	SpriteRenderer* gradientSprRF = gradientGameObjectFront->AddComponent<SpriteRenderer>();
	gradientSprRF->texture = gradient;
	gradientSprRF->material = material2D;
	gradientSprRF->color = Color::CreateFromRGBAFloat(1, 1, 1, 1.0f);
	//gradientGameObjectFront->AddExistingComponent(gradientSprRF);
	gradientGameObjectFront->transform.SetPosition(Vector3(2, 1, 1));

	camera->SetProjectionType(Orthographic);
	camera->SetProjectionSize(2.5f * 1);
}

void RenderingTest2D::LoadGameData()
{
	Shader* shader = new Shader("vertexStandard.shader", "fragmentStandard.shader");

	Shader* shader3 = new Shader("3D/vStandard.shader", "3D/fStandard.shader");

	Shader* shader2 = new Shader("vertex2.shader", "fragment2.shader");
	Shader* shaderText = new Shader("vertexText.shader", "fragmentText.shader");
	Shader* shaderTextCanvas = new Shader("vertexTextCanvas.shader", "fragmentTextCanvas.shader");
	Shader* shader2D = new Shader("vertex2D.shader", "fragment2D.shader");
	Shader* shaderStandard2D = new Shader("vertexStandard2D.shader", "fragmentStandard2D.shader");
	Shader* shaderStandard2DText = new Shader("vertexStandard2DText.shader", "fragmentStandard2DText.shader");
	Shader* shaderStandard2DWithZ = new Shader("vertexStandard2DZ.shader", "fragmentStandard2DZ.shader");

	textureTile0 = new Texture("rts/Tile/scifiTile_42.png", "Ground0");
	textureTile0->SetPixelPerUnit(128);

	crosshair = new Texture("rts/crosshairs/crosshair.png", "Crosshair");
	crosshair->SetPixelPerUnit(128);

	gradient = new Texture("gradient.png", "gradient");
	gradient->SetWrapMode(Texture::ClampToEdge);

	material2D = new Material("2D Standard");
	material2D->shader = shaderStandard2D;

	material2DWithZ = new Material("2D Standard With Z");
	material2DWithZ->shader = shaderStandard2DWithZ;
}

void RenderingTest2D::Loop()
{
	Vector2 mouseWorldPosition = camera->MouseTo2DWorld();
	gradientGameObject->transform.SetPosition(Vector3(mouseWorldPosition.x, mouseWorldPosition.y+0.5, 0));
	tileGameObject->transform.SetPosition(Vector3(mouseWorldPosition.x, mouseWorldPosition.y-0.5, 0));

	//Move camera
	float cameraArrowMoveSpeed = 3;
	Vector3 newCameraPosition = camera->GetGameObject()->transform.GetPosition();
	if (InputSystem::GetKey(Z))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetUp();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(S))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetDown();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(D))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetRight();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	if (InputSystem::GetKey(Q))
	{
		Vector3 vect = Graphics::usedCamera->GetGameObject()->transform.GetLeft();
		vect *= Time::GetDeltaTime() * cameraArrowMoveSpeed;
		newCameraPosition += vect;
	}
	camera->GetGameObject()->transform.SetPosition(newCameraPosition);
}
