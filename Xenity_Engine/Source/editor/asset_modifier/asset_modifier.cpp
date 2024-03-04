#include "asset_modifier.h"

#include <engine/graphics/texture.h>
#include <engine/file_system/file.h>

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image_resize.h>
#include <engine/debug/debug.h>

void AssetModifier::CropTexture(std::shared_ptr<Texture> textureInput, const int posX, const int posY, const int width, const int height, std::shared_ptr<File> fileOutput)
{
	const bool openResult = textureInput->file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		// Read texture file's data
		int fileBufferSize;
		unsigned char* fileData = textureInput->file->ReadAllBinary(fileBufferSize);
		textureInput->file->Close();

		if (!fileData) 
		{
			Debug::PrintError("[AssetModifier::CropTexture] Fail to read file data", true);
			return;
		}

		int channelCount = 4;

		// Load image with stb_image
		int originalWidth, originalHeight, nrChannels;
		unsigned char* buffer = stbi_load_from_memory(fileData, fileBufferSize, &originalWidth, &originalHeight,
			&nrChannels, channelCount);

		free(fileData);
		if (!buffer)
		{
			Debug::PrintError("[AssetModifier::CropTexture] stbi_load_from_memory failed", true);
			return;
		}

		// Alloc memory for the cropped texture
		unsigned char* croppedBuffer = (unsigned char*)malloc(width * height * channelCount * sizeof(unsigned char));
		if (!croppedBuffer)
		{
			stbi_image_free(buffer);
			Debug::PrintError("[AssetModifier::CropTexture] Fail to allocate memory for croppedBuffer", true);
			return;
		}
		
		// Copy needed texture data to the cropped texture buffer
		int originalTexDatIndex;
		int croppedTexDatIndex;
		const int yOffset = originalHeight - height;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width * channelCount; x++)
			{
				originalTexDatIndex = (x + posX * channelCount) + (y + yOffset - posY) * (originalWidth * channelCount);
				croppedTexDatIndex = x + y * (width * channelCount);
				croppedBuffer[croppedTexDatIndex] = buffer[originalTexDatIndex];
			}
		}

		stbi_write_png(fileOutput->GetPath().c_str(), width, height, channelCount, croppedBuffer, 0);

		free(croppedBuffer);
		stbi_image_free(buffer);
	}
	else 
	{
		Debug::PrintError("[AssetModifier::CropTexture] Fail to open file", true);
	}
}
