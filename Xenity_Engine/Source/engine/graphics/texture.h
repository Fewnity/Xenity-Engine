#pragma once
#include <engine/api.h>

#include <string>
#include <vector>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/2d_graphics/sprite_selection.h>

#if defined(_EE)
#include <draw.h>
// #include <gsKit.h>
#endif

ENUM(Filter, Point, Bilinear);
ENUM(AnisotropicLevel, X0, X2, X4, X8, X16);
ENUM(WrapMode, ClampToEdge, ClampToBorder, MirroredRepeat, Repeat, MirrorClampToEdge);
ENUM(PSPTextureType, RGBA_8888, RGBA_5551, RGBA_5650, RGBA_4444);

class API Texture : public FileReference, public Reflective
{
public:
	Texture();

	/**
	 * [Internal]
	 */
	// Texture(unsigned char* data, const int channelCount, const int width, const int height, bool loadInVram);

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;

	static std::shared_ptr<Texture> MakeTexture();
	// static std::shared_ptr<Texture> MakeTexture(unsigned char* data, const int channelCount, const int width, const int height, bool loadInVram);

	~Texture();

	void LoadFileReference() override;
	void OnLoadFileReferenceFinished() override;
	void UnloadFileReference() override;

	/**
	 * Set texture data
	 * @param data Texture data
	 */
	void SetData(const unsigned char *data);

	/**
	 * [Internal] Get texture ID
	 */
	unsigned int GetTextureId() const;

	/**
	 * Set texture filter
	 * @param filter Filter
	 */
	void SetFilter(const Filter filter);

	/**
	 * Set texture wrap mode
	 * @param mode Wrap mode
	 */
	void SetWrapMode(const WrapMode mode);

	void SetSize(int width, int height)
	{
		this->width = width;
		this->height = height;
	}
	/**
	 * Get texture width
	 */
	int GetWidth() const;

	/**
	 * Get texture height
	 */
	int GetHeight() const;

	/**
	 * Set texture pixel per unit
	 * @param value Pixel per unit
	 */
	void SetPixelPerUnit(int value);

	/**
	 * Get texture pixel per unit
	 */
	int GetPixelPerUnit() const;

	/**
	 * Get texture channel count
	 */
	int GetChannelCount() const;

	void SetChannelCount(int channelCount)
	{
		this->nrChannels = channelCount;
	}

	/**
	 * Get if the texture is using mipmap
	 */
	bool GetUseMipmap() const;

	/**
	 * Get texture filter
	 */
	Filter GetFilter() const;

	/**
	 * Get texture wrap mode
	 */
	WrapMode GetWrapMode() const;

	int mipmaplevelCount = 0;

	/**
	 * Return if the texture is valid
	 */
	bool IsValid() const
	{
		return isValid;
	}

	/**
	 * [Internal]
	 */
	std::shared_ptr<Texture> GetThisShared()
	{
		return std::dynamic_pointer_cast<Texture>(shared_from_this());
	}

#if defined(__PSP__)
	/**
	 * Create texture data for a mipmap level
	 * @param level Mipmap level
	 * @param texData Base texture data
	 */
	void SetTextureLevel(int level, const unsigned char *texData);

	std::vector<void *> data;
	std::vector<bool> inVram;

	unsigned int pW = 0;
	unsigned int pH = 0;
#elif defined(_EE)
	// GSTEXTURE ps2Tex;
	texbuffer_t texbuff;
#endif
	PSPTextureType type = PSPTextureType::RGBA_5650;
	std::vector<SpriteSelection*> spriteSelections;
	void ClearSpriteSelections();

private:
	/**
	 * Create the texture from a file path
	 * @param filePath File path
	 * @param filter Texture filter
	 * @param useMipMap Use mip map
	 */
	void CreateTexture(const Filter filter, const bool useMipMap);

	/**
	 * Load texture data
	 */
	void LoadTexture();

	/**
	 * Unload texture data
	 */
	void Unload();

	unsigned char *buffer = nullptr;
	Filter filter = Filter::Bilinear;
	WrapMode wrapMode = WrapMode::Repeat;
	unsigned int textureId = 0;
	int width = 0, height = 0, nrChannels = 0;

	int pixelPerUnit = 100;
	bool isValid = false;
	public:
		bool useMipMap = false;
};
