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
	spriteRenderer = GetGameObject()->AddComponent<SpriteRenderer>();
	spriteRenderer->texture = unitData->bulletTexture;
	spriteRenderer->material = Game::GetGame()->material2DWithZ;
	spriteRenderer->SetOrderInLayer(4);
}

void Bullet::Update()
{
	Vector3 newPos = GetTransform()->GetPosition();
	newPos += direction * 1 * Time::GetDeltaTime();
	GetTransform()->SetPosition(newPos);
}
