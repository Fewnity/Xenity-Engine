#include "building.h"
#include <iostream>
#include "../rts_game/building_data.h"
#include "../xenity.h"
#include "game.h"
#include "prop.h"
#include "prop_data.h"

Building::Building(BuildingData* data, BuildManager* buildManager)
{
	buildingData = data;

}

void Building::Start()
{
	GameObject* gmBuildingSprite = new GameObject();
	SpriteRenderer* buildingSpriteRenderer = new SpriteRenderer(buildingData->textures[0], AssetManager::GetMaterialByName("2D Standard"));
	buildingSpriteRenderer->orderInLayer = 2;
	gmBuildingSprite->AddExistingComponent(buildingSpriteRenderer);
	selectionSpriteRenderer = new SpriteRenderer(buildingData->selectionTexture, AssetManager::GetMaterialByName("2D Standard"));
	selectionSpriteRenderer->orderInLayer = 11;
	gmBuildingSprite->AddExistingComponent(selectionSpriteRenderer);
	selectionSpriteRenderer->SetIsEnabled(false);
	gameObject->AddChild(gmBuildingSprite);
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