#include "building.h"
#include <iostream>
#include "../rts_game/building_data.h"
#include "../xenity.h"

Building::Building(BuildingData* data, BuildManager* buildManager)
{
	buildingData = data;

}

void Building::Start()
{
	GameObject* gmBuildingSprite = new GameObject();
	SpriteRenderer* unitSpriteRenderer = new SpriteRenderer(buildingData->textures[0], AssetManager::GetMaterialByName("2D Standard"));
	gmBuildingSprite->AddExistingComponent(unitSpriteRenderer);
	selectionSpriteRenderer = new SpriteRenderer(buildingData->selectionTexture, AssetManager::GetMaterialByName("2D Standard"));
	gmBuildingSprite->AddExistingComponent(selectionSpriteRenderer);
	selectionSpriteRenderer->SetIsEnabled(false);
	gameObject->AddChild(gmBuildingSprite);
	gmBuildingSprite->transform.SetLocalPosition(Vector3(0, 0, 0));
	gmBuildingSprite->transform.SetLocalScale(1.0f);
}

void Building::Update()
{
	
}