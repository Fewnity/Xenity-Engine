#pragma once

#include <string>


class Texture
{
public:
	enum Filter 
	{
		Point,
		Bilinear,
	};

	enum AnisotropicLevel 
	{
		X0,
		X2,
		X4,
		X8,
		X16,
	};

	enum WrapMode 
	{
		Repeat,
		Clamp,
	};

	Texture() = delete;
	Texture(const std::string filePath, std::string name);
	Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap);

	~Texture();
	unsigned int GetTextureId() const;
	void UpdateTextureFilter();
	void SetFilter(const Filter filter);
	void SetWrapMode(const WrapMode mode);
	int GetWidth() const;
	int GetHeight() const;
	void SetPixelPerUnit(int value);
	int GetPixelPerUnit() const;
	std::string name = "";

private:
	Filter filter = Bilinear;
	WrapMode wrapMode = Repeat;
	bool useMipMap = true;
	void CreateTexture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap);
	void LoadTexture(const std::string filePath);
	unsigned int textureId;
	int width = 0, height = 0, nrChannels = 0;
	int pixelPerUnit = 100;
};

