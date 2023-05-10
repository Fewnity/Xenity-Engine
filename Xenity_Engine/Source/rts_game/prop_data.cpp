#include "prop_data.h"
#include "../engine/graphics/texture.h"
#include "../engine/engine_settings.h"

void PropData::AddPropData(float miningRate, std::string textureName)
{
	Data* newData= new Data();
	newData->miningRate = miningRate;
	newData->texture = new Texture(textureName, "", Texture::Point, false);
	newData->texture->SetPixelPerUnit(128);
	newData->texture->SetWrapMode(Texture::ClampToEdge);
	data.push_back(newData);
}
