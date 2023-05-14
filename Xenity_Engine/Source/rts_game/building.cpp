#include "building.h"
#include <iostream>
#include "../rts_game/building_data.h"
#include "../xenity.h"
#include "game.h"
#include "prop.h"
#include "prop_data.h"

Building::Building()
{
	componentName = "Building";
}

void Building::Start()
{
	std::cout << "UWU" << std::endl;

	GameObject* gmBuildingSprite = new GameObject("Building Sprite");
	SpriteRenderer* buildingSpriteRenderer = gmBuildingSprite->AddComponent<SpriteRenderer>();
	buildingSpriteRenderer->texture = buildingData->textures[0];
	buildingSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	buildingSpriteRenderer->SetOrderInLayer(2);

	selectionSpriteRenderer = gmBuildingSprite->AddComponent<SpriteRenderer>();
	selectionSpriteRenderer->texture = buildingData->selectionTexture;
	selectionSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	selectionSpriteRenderer->SetOrderInLayer(11);
	selectionSpriteRenderer->SetIsEnabled(false);
	GetGameObject()->AddChild(gmBuildingSprite);
	gmBuildingSprite->transform.SetLocalPosition(Vector3(0, 0, 0));
	gmBuildingSprite->transform.SetLocalScale(1.0f);
}

void Building::Update()
{
	if (buildingData->type == BuildingType::Miner)
	{
		miningTimer += Time::GetDeltaTime();
		if (miningTimer >= tile->prop->data->data[tile->prop->id]->miningRate) 
		{
			miningTimer = 0;
			if (tile->prop->data->type == Wood)
				Game::GetGame()->wood++;
			else if (tile->prop->data->type == Rock)
				Game::GetGame()->rock++;
			else if (tile->prop->data->type == Gold)
				Game::GetGame()->gold++;
			else if (tile->prop->data->type == Crystal)
				Game::GetGame()->crystal++;
		}
	}
}