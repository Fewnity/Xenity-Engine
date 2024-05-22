// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <memory>

class Texture;
class File;

/**
* @brief Class to modify assets (Crop textures)
*/
class AssetModifier
{
public:

	/**
	* @brief Create a cropped texture from a texture
	*/
	static void CropTexture(std::shared_ptr<Texture> textureInput, const int posX, const int posY, const int width, const int heigh, std::shared_ptr<File> fileOutput);
};

