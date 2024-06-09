// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

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

/**
* @brief Texture file class
*/
class API Texture : public FileReference, public Reflective
{
public:
	Texture();
	~Texture();

	/**
	 * @brief Set texture filter
	 * @param filter Filter
	 */
	void SetFilter(const Filter filter);

	/**
	 * @brief Set texture wrap mode
	 * @param mode Wrap mode
	 */
	void SetWrapMode(const WrapMode mode);

	inline void SetSize(int width, int height)
	{
		this->width = width;
		this->height = height;
	}

	/**
	 * @brief Get texture width
	 */
	int GetWidth() const;

	/**
	 * @brief Get texture height
	 */
	int GetHeight() const;

	/**
	 * @brief Set texture pixel per unit
	 * @param value Pixel per unit
	 */
	void SetPixelPerUnit(int value);

	/**
	 * @brief Get texture pixel per unit
	 */
	int GetPixelPerUnit() const;

	/**
	 * @brief Get if the texture is using mipmap
	 */
	bool GetUseMipmap() const;

	/**
	 * @brief Get texture filter
	 */
	Filter GetFilter() const;

	/**
	 * @brief Get texture wrap mode
	 */
	WrapMode GetWrapMode() const;

protected:
	template <class T>
	friend class SelectAssetMenu;
	friend class RendererOpengl;
	friend class RendererGU;
	friend class RendererGsKit;
	friend class RendererVU1;
	friend class EditorUI;
	friend class Editor;
	friend class SpriteEditorMenu;
	friend class BuildSettingsMenu;
	friend class FileExplorerMenu;
	friend class SceneMenu;
	friend class InspectorMenu;
	friend class MainBarMenu;
	friend class Font;
	friend class AssetManager;
	friend class ProjectManager;
	friend class TextManager;

	/**
	* @brief Get texture channel count
	*/
	int GetChannelCount() const;

	inline void SetChannelCount(int channelCount)
	{
		this->nrChannels = channelCount;
	}

	/**
	 * @brief Return if the texture is valid
	 */
	inline bool IsValid() const
	{
		return isValid;
	}

	/**
	* @brief [Internal] Get mipmap level count
	* @return 0 if mipmapping is not used
	*/
	inline int GetMipmaplevelCount() const
	{
		return mipmaplevelCount;
	}

	/**
	* @bried [Internal] Get if the texture is using mipmapping
	*/
	inline bool GetIsUsingMipMap() const
	{
		return useMipMap;
	}

	/**
	 * @brief [Internal]
	 */
	inline std::shared_ptr<Texture> GetThisShared()
	{
		return std::dynamic_pointer_cast<Texture>(shared_from_this());
	}

#if defined(__PSP__)
	/**
	 * @brief Create texture data for a mipmap level
	 * @param level Mipmap level
	 * @param texData Base texture data
	 */
	void SetTextureLevel(int level, const unsigned char *texData);

	// One vector element for each mipmap level
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

	/**
	* @brief Clear all sprite selections
	*/
	void ClearSpriteSelections();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData() override;

	static std::shared_ptr<Texture> MakeTexture();

	void LoadFileReference() override;
	void OnLoadFileReferenceFinished() override;
	void UnloadFileReference() override;

	/**
	 * @brief Set texture data
	 * @param data Texture data
	 */
	void SetData(const unsigned char* data);

	/**
	 * @brief [Internal] Get texture ID
	 */
	unsigned int GetTextureId() const;

	int mipmaplevelCount = 0;

	/**
	 * @brief Create the texture
	 * @param filter Texture filter
	 * @param useMipMap Use mip map
	 */
	void CreateTexture(const Filter filter, const bool useMipMap);

	/**
	 * @brief Load texture data
	 */
	void LoadTexture();

	/**
	 * @brief Unload texture data
	 */
	void Unload();

	unsigned char *buffer = nullptr;
	Filter filter = Filter::Bilinear;
	WrapMode wrapMode = WrapMode::Repeat;
	unsigned int textureId = 0;
	int width = 0, height = 0, nrChannels = 0;

	int pixelPerUnit = 100;
	bool isValid = false;
	bool useMipMap = false;
public:
};
