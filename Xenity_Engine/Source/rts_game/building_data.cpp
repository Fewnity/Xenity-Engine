#include "building_data.h"
#include "../xenity.h"

BuildingData::BuildingData(int index, Texture* selectionTexture)
{
	int textureNameIndex = index + 1;
	//Load blue units
	std::string buildingTextureName = "rts/Structure/blueStructure_" + std::to_string(textureNameIndex) + ".png";
	textures[0] = new Texture(buildingTextureName, "", Texture::Bilinear, false);
	textures[0]->SetPixelPerUnit(128);

	//Load green units
	buildingTextureName = "rts/Structure/greenStructure_" + std::to_string(textureNameIndex) + ".png";
	textures[1] = new Texture(buildingTextureName, "", Texture::Bilinear, false);
	textures[1]->SetPixelPerUnit(128);

	//Load grey units
	buildingTextureName = "rts/Structure/greyStructure_" + std::to_string(textureNameIndex) + ".png";
	textures[2] = new Texture(buildingTextureName, "", Texture::Bilinear, false);
	textures[2]->SetPixelPerUnit(128);
	
	//Load orange units
	buildingTextureName = "rts/Structure/orangeStructure_" + std::to_string(textureNameIndex) + ".png";
	textures[3] = new Texture(buildingTextureName, "", Texture::Bilinear, false);
	textures[3]->SetPixelPerUnit(128);

	this->selectionTexture = selectionTexture;
}