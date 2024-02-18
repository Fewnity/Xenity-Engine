#pragma once

#include <memory>

class Texture;
class File;

class AssetModifier
{
public:
	static void CropTexture(std::shared_ptr<Texture> textureInput, int posX, int posY, int width, int heigh, std::shared_ptr<File> fileOutput);
};

