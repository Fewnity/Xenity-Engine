#include "unit_data.h"
#include "../graphics/texture.h"
#include "../engine_settings.h"

UnitData::UnitData(int index, Texture* selectionTexture)
{
	EngineSettings::anisotropicLevel = Texture::X0;

	int textureNameIndex = index + 1;
	//Load blue ints
	std::string unitTextureName = "rts/Unit/blueUnit_" + std::to_string(textureNameIndex) + ".png";
	textures[0] = new Texture(unitTextureName, "", Texture::Point, false);
	textures[0]->SetPixelPerUnit(128);

	//Load green units
	unitTextureName = "rts/Unit/greenUnit_" + std::to_string(textureNameIndex) + ".png";
	textures[1] = new Texture(unitTextureName, "", Texture::Point, false);
	textures[1]->SetPixelPerUnit(128);

	//Load grey units
	unitTextureName = "rts/Unit/greyUnit_" + std::to_string(textureNameIndex) + ".png";
	textures[2] = new Texture(unitTextureName, "", Texture::Point, false);
	textures[2]->SetPixelPerUnit(128);
	
	//Load orange units
	unitTextureName = "rts/Unit/orangeUnit_" + std::to_string(textureNameIndex) + ".png";
	textures[3] = new Texture(unitTextureName, "", Texture::Point, false);
	textures[3]->SetPixelPerUnit(128);

	this->selectionTexture = selectionTexture;
}
