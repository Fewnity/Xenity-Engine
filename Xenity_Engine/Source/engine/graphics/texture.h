// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <vector>
#include <map>
#if defined(_EE)
#include <draw.h>
// #include <gsKit.h>
#endif

#include <engine/api.h>
#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>
#include <engine/graphics/2d_graphics/sprite_selection.h>
#include <engine/reflection/enum_utils.h>
#include <engine/platform.h>
#include <engine/application.h>

ENUM(Filter, Point, Bilinear);
ENUM(AnisotropicLevel, X0, X2, X4, X8, X16);
ENUM(TextureResolutions, R_64x64 = 64, R_128x128 = 128, R_256x256 = 256, R_512x512 = 512, R_1024x1024 = 1024, R_2048x2048 = 2048);
ENUM(WrapMode, ClampToEdge, ClampToBorder, MirroredRepeat, Repeat, MirrorClampToEdge);
ENUM(PSPTextureType, RGBA_8888, RGBA_5551, RGBA_5650, RGBA_4444);
ENUM(PS3TextureType, ARGB_8888, ARGB_1555, ARGB_0565, ARGB_4444);

class TextureSettings : public Reflective
{
public:
	TextureResolutions resolution = TextureResolutions::R_2048x2048;
	Filter filter = Filter::Bilinear;
	WrapMode wrapMode = WrapMode::Repeat;
	bool useMipMap = false;
	int mipmaplevelCount = 0;
	int pixelPerUnit = 100;

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution", true);
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap", true);
		Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
		Reflective::AddVariable(reflectedVariables, filter, "filter", true);
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode", true);
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit", true);
		return reflectedVariables;
	}
};

class TextureSettingsStandalone : public TextureSettings
{
public:
};

class TextureSettingsPSVITA : public TextureSettings
{
public:
};

class TextureSettingsPSP : public TextureSettings
{
public:
	PSPTextureType type = PSPTextureType::RGBA_5650;
	bool tryPutInVram = true;

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution", true);
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap", true);
		Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
		Reflective::AddVariable(reflectedVariables, filter, "filter", true);
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode", true);
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit", true);
		Reflective::AddVariable(reflectedVariables, type, "type", true);
		Reflective::AddVariable(reflectedVariables, tryPutInVram, "tryPutInVram", true);
		return reflectedVariables;
	}
};

class TextureSettingsPS3 : public TextureSettings
{
public:
	PS3TextureType type = PS3TextureType::ARGB_0565;

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution", true);
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap", true);
		Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
		Reflective::AddVariable(reflectedVariables, filter, "filter", true);
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode", true);
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit", true);
		Reflective::AddVariable(reflectedVariables, type, "type", true);
		return reflectedVariables;
	}
};

/**
* @brief Texture file class
*/
class API Texture : public FileReference
{
public:
	Texture();

	inline void SetSize(int width, int height)
	{
		this->m_width = width;
		this->height = height;
	}

	/**
	 * @brief Set texture filter
	 * @param filter Filter
	 */
	inline void SetFilter(const Filter filter)
	{
		m_settings.at(Application::GetAssetPlatform())->filter = filter;
	}

	/**
	 * @brief Set texture wrap mode
	 * @param mode Wrap mode
	 */
	inline void SetWrapMode(const WrapMode mode)
	{
		m_settings.at(Application::GetAssetPlatform())->wrapMode = mode;
	}

	/**
	 * @brief Get texture width
	 */
	inline int GetWidth() const
	{
		return m_width;
	}

	/**
	 * @brief Get texture height
	 */
	inline int GetHeight() const
	{
		return height;
	}

	/**
	 * @brief Set texture pixel per unit
	 * @param value Pixel per unit
	 */
	inline void SetPixelPerUnit(int value)
	{
		m_settings.at(Application::GetAssetPlatform())->pixelPerUnit = value;
	}

	/**
	 * @brief Get texture pixel per unit
	 */
	inline int GetPixelPerUnit() const
	{
		return m_settings.at(Application::GetAssetPlatform())->pixelPerUnit;
	}

	/**
	 * @brief Get if the texture is using mipmap
	 */
	inline bool GetUseMipmap() const
	{
		return m_settings.at(Application::GetAssetPlatform())->useMipMap;
	}

	/**
	 * @brief Get texture filter
	 */
	inline Filter GetFilter() const
	{
		return m_settings.at(Application::GetAssetPlatform())->filter;
	}
	
	/**
	 * @brief Get texture wrap mode
	 */
	inline WrapMode GetWrapMode() const
	{
		return m_settings.at(Application::GetAssetPlatform())->wrapMode;
	}

protected:
	template <class T>
	friend class SelectAssetMenu;
	friend class RendererOpengl;
	friend class RendererRSX;
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
	friend class Cooker;

	/**
	* @brief Get texture channel count
	*/
	inline int GetChannelCount() const
	{
		return nrChannels;
	}

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
		return m_settings.at(Application::GetAssetPlatform())->mipmaplevelCount;
	}

	inline TextureResolutions GetCookResolution() const
	{
		return m_settings.at(Application::GetAssetPlatform())->resolution;
	}

	/**
	 * @brief [Internal]
	 */
	inline std::shared_ptr<Texture> GetThisShared()
	{
		return std::dynamic_pointer_cast<Texture>(shared_from_this());
	}

#if defined(_EE)
	// GSTEXTURE ps2Tex;
	texbuffer_t texbuff;
#endif
	std::vector<SpriteSelection*> spriteSelections;

	/**
	* @brief Clear all sprite selections
	*/
	void ClearSpriteSelections();

	ReflectiveData GetReflectiveData() override;
	ReflectiveData GetMetaReflectiveData(AssetPlatform platform) override;
	void OnReflectionUpdated() override;

	static std::shared_ptr<Texture> MakeTexture();

	void LoadFileReference(const LoadOptions& loadOptions) override;
	void UnloadFileReference() override;

	/**
	 * @brief Set texture data
	 * @param data Texture data
	 */
	virtual void SetData(const unsigned char* data) = 0;

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
	virtual void Unload() = 0;
	virtual void Bind() const = 0;

	std::map<AssetPlatform, std::unique_ptr<TextureSettings>> m_settings;
	unsigned char* m_buffer = nullptr;
	int m_width = 0, height = 0, nrChannels = 0;

#if defined(EDITOR)
	TextureResolutions previousResolution = TextureResolutions::R_2048x2048;
#endif
	bool isValid = false;
};
