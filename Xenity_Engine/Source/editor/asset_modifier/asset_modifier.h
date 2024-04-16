#pragma once

#include <memory>

class Texture;
class File;

/**
* Class to modify assets (Crop textures)
*/
class AssetModifier
{
public:

	/**
	* Create a cropped texture from a texture
	*/
	static void CropTexture(std::shared_ptr<Texture> textureInput, const int posX, const int posY, const int width, const int heigh, std::shared_ptr<File> fileOutput);
};

