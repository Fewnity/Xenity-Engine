#include "bullet.h"
#include "../xenity.h"
#include "game.h"

Bullet::Bullet()
{
	reflectedFloats.insert(std::pair<std::string, float*>("Speed", &speed));
	componentName = "Bullet";
}

void Bullet::Start()
{
	spriteRenderer = GetGameObject()->AddComponent<SpriteRenderer>();
	spriteRenderer->texture = AssetManager::defaultTexture;
	spriteRenderer->material = Game::GetGame()->material2DWithZ;
	spriteRenderer->SetOrderInLayer(4);
}

void Bullet::Update()
{
	Vector3 newPos = GetTransform()->GetPosition();
	newPos += direction * speed * Time::GetDeltaTime();
	GetTransform()->SetPosition(newPos);
}
