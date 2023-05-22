#include "bullet.h"
#include "../xenity.h"
#include "game.h"
#include "unit_data.h"

Bullet::Bullet()
{
	//reflectedFloats.insert(std::pair<std::string, float*>("Speed", &speed));
	componentName = "Bullet";
}

void Bullet::Start()
{
	//Create bullet sprite
	spriteRenderer = GetGameObject()->AddComponent<SpriteRenderer>();
	spriteRenderer->texture = unitData->bulletTexture;
	spriteRenderer->material = Game::GetGame()->material2DWithZ;
	spriteRenderer->SetOrderInLayer(4);
}

void Bullet::Update()
{
	//Move the bullet
	Vector3 newPos = GetTransform()->GetPosition();
	newPos += direction * unitData->bulletSpeed * Time::GetDeltaTime();
	GetTransform()->SetPosition(newPos);
}
