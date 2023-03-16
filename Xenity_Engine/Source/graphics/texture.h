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
	Texture(const std::string filePath);
	Texture(const std::string filePath, const Filter filter, const bool useMipMap);

	~Texture();
	unsigned int GetTextureId() const;
	void UpdateTextureFilter();
	void SetFilter(const Filter filter);
	void SetWrapMode(const WrapMode mode);
	int GetWidth() const;
	int GetHeight() const;
	void SetPixelPerUnit(int value);
	int GetPixelPerUnit() const;

private:
	Filter filter = Bilinear;
	WrapMode wrapMode = Repeat;
	bool useMipMap = true;
	void CreateTextutre(const std::string filePath, const Filter filter, const bool useMipMap);
	void LoadTexture(const std::string filePath);
	unsigned int textureId;
	int width = 0, height = 0, nrChannels = 0;
	int pixelPerUnit = 100;
};

