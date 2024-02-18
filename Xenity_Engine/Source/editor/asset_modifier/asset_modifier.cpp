#include "asset_modifier.h"

#include <engine/graphics/texture.h>
#include <engine/file_system/file.h>

#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_image_resize.h>
#include <engine/debug/debug.h>

void AssetModifier::CropTexture(std::shared_ptr<Texture> textureInput, int posX, int posY, int width, int height, std::shared_ptr<File> fileOutput)
{
	const bool openResult = textureInput->file->Open(FileMode::ReadOnly);
	if (openResult)
	{
		int fileBufferSize;
		unsigned char* fileData = textureInput->file->ReadAllBinary(fileBufferSize);

		if (!fileData) 
		{
			Debug::PrintError("[AssetModifier::CropTexture] Fail to read file data");
			return;
		}

		textureInput->file->Close();
		int channelCount = 4;

		// Load image with stb_image
		int originalWidth, originalHeight, nrChannels;
		unsigned char* buffer = stbi_load_from_memory(fileData, fileBufferSize, &originalWidth, &originalHeight,
			&nrChannels, channelCount);

		free(fileData);
		if (!buffer)
		{
			Debug::PrintError("[AssetModifier::CropTexture] stbi_load_from_memory failed");
			return;
		}

		unsigned char* croppedBuffer = (unsigned char*)malloc(width * height * channelCount * sizeof(unsigned char));
		if (!croppedBuffer)
		{
			stbi_image_free(buffer);
			Debug::PrintError("[AssetModifier::CropTexture] Fail to allocate memory for croppedBuffer");
			return;
		}

		int index;
		int index2;
		int yOffset = originalHeight - height;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width * channelCount; x++)
			{
				index = (x + posX * channelCount) + (y + yOffset - posY) * (originalWidth * channelCount);
				index2 = x + y * (width * channelCount);
				croppedBuffer[index2] = buffer[index];
			}
		}

		stbi_write_png(fileOutput->GetPath().c_str(), width, height, channelCount, croppedBuffer, 0);

		free(croppedBuffer);
		stbi_image_free(buffer);
	}
	else 
	{
		Debug::PrintError("[AssetModifier::CropTexture] Fail to open file");
	}
}
