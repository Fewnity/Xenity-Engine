#pragma once

#include <string>


class Texture
{
public:
	enum Filter {
		Point,
		Bilinear,
	};

	enum AnisotropicLevel {
		X0,
		X2,
		X4,
		X8,
		X16,
	};

	Texture() = delete;
	Texture(std::string filePath);
	Texture(std::string filePath, Filter filter, bool useMipMap);

	~Texture();
	unsigned int GetTextureIndex();
	unsigned int GetTextureId();
	void UpdateTextureFilter();
	void SetFilter(Filter filter);

private:
	Filter filter = Bilinear;
	bool useMipMap = true;
	void CreateTextutre(std::string filePath, Filter filter, bool useMipMap);
	void LoadTexture(std::string filePath);
	unsigned int textureId, textureIndex;
	int width = 0, height = 0, nrChannels = 0;
};

