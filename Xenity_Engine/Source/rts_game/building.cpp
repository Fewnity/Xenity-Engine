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
	//Create building sprite
	GameObject* gmBuildingSprite = new GameObject("Building Sprite");
	GetGameObject()->AddChild(gmBuildingSprite);
	gmBuildingSprite->GetTransform()->SetLocalPosition(Vector3(0, 0, 0));
	SpriteRenderer* buildingSpriteRenderer = gmBuildingSprite->AddComponent<SpriteRenderer>();
	buildingSpriteRenderer->texture = buildingData->textures[0];
	buildingSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	buildingSpriteRenderer->SetOrderInLayer(2);

	//Create selection sprite
	selectionSpriteRenderer = gmBuildingSprite->AddComponent<SpriteRenderer>();
	selectionSpriteRenderer->texture = buildingData->selectionTexture;
	selectionSpriteRenderer->material = AssetManager::GetMaterialByName("2D Standard");
	selectionSpriteRenderer->SetOrderInLayer(11);
	selectionSpriteRenderer->SetIsEnabled(false);
}

void Building::Update()
{
	//If the building is a miner, mine some ressources
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