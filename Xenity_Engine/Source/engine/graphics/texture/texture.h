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
//ENUM(AnisotropicLevel, X0, X2, X4, X8, X16); // Not used for now, but could be useful later
ENUM(TextureResolution, R_64x64 = 64, R_128x128 = 128, R_256x256 = 256, R_512x512 = 512, R_1024x1024 = 1024, R_2048x2048 = 2048);
ENUM(WrapMode, ClampToEdge = 0, Repeat = 3);
ENUM(PSPTextureType, RGBA_8888, RGBA_5551, RGBA_5650, RGBA_4444);
ENUM(PS3TextureType, ARGB_8888, ARGB_1555, ARGB_0565, ARGB_4444);

class TextureSettings : public Reflective
{
public:
	TextureResolution resolution = TextureResolution::R_2048x2048;
	Filter filter = Filter::Bilinear;
	WrapMode wrapMode = WrapMode::Repeat;
	bool useMipMap = false;
	//int mipmaplevelCount = 0;
	int pixelPerUnit = 100;

	void OnReflectionUpdated() override
	{
		if(pixelPerUnit <= 0)
		{
			pixelPerUnit = 1; // Ensure pixel per unit is always positive
		}
	}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution");
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap");
		//Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount");
		Reflective::AddVariable(reflectedVariables, filter, "filter");
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode");
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit");
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
	TextureSettingsPSVITA() 
	{
		resolution = TextureResolution::R_256x256;
	}
};

class TextureSettingsPSP : public TextureSettings
{
public:
	TextureSettingsPSP()
	{
		resolution = TextureResolution::R_128x128;
	}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution");
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap");
		//Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount", true);
		Reflective::AddVariable(reflectedVariables, filter, "filter");
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode");
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit");
		Reflective::AddVariable(reflectedVariables, type, "type");
		Reflective::AddVariable(reflectedVariables, tryPutInVram, "tryPutInVram");
		return reflectedVariables;
	}

	PSPTextureType type = PSPTextureType::RGBA_5650;
	bool tryPutInVram = true;
};

class TextureSettingsPS3 : public TextureSettings
{
public:
	TextureSettingsPS3()
	{
		resolution = TextureResolution::R_256x256;
	}

	ReflectiveData GetReflectiveData() override
	{
		ReflectiveData reflectedVariables;
		Reflective::AddVariable(reflectedVariables, resolution, "resolution");
		Reflective::AddVariable(reflectedVariables, useMipMap, "useMipMap");
		//Reflective::AddVariable(reflectedVariables, mipmaplevelCount, "mipmaplevelCount");
		Reflective::AddVariable(reflectedVariables, filter, "filter");
		Reflective::AddVariable(reflectedVariables, wrapMode, "wrapMode");
		Reflective::AddVariable(reflectedVariables, pixelPerUnit, "pixelPerUnit");
		Reflective::AddVariable(reflectedVariables, type, "type");
		return reflectedVariables;
	}

	PS3TextureType type = PS3TextureType::ARGB_0565;
};

struct TextureConstructorParams
{
	unsigned int width = 0;
	unsigned int height = 0;
	bool hasAlpha = true;
	Filter filter = Filter::Bilinear;
	WrapMode wrapMode = WrapMode::Repeat;
	PSPTextureType pspTextureType = PSPTextureType::RGBA_5650;
	PS3TextureType ps3TextureType = PS3TextureType::ARGB_0565;
};

/**
* @brief Texture file class
*/
class API Texture : public FileReference
{
public:
	Texture();

	/**
	* @brief Dynamically create a texture
	*/
	[[nodiscard]] static std::shared_ptr<Texture> CreateTexture(const TextureConstructorParams& params);

	/**
	 * @brief Set texture filter
	 * @param filter Filter
	 */
	void SetFilter(const Filter filter)
	{
		m_settings.at(Application::GetAssetPlatform())->filter = filter;
	}

	/**
	 * @brief Set texture wrap mode
	 * @param mode Wrap mode
	 */
	void SetWrapMode(const WrapMode mode)
	{
		m_settings.at(Application::GetAssetPlatform())->wrapMode = mode;
	}

	/**
	 * @brief Get texture width
	 */
	[[nodiscard]] int GetWidth() const
	{
		return m_width;
	}

	/**
	 * @brief Get texture height
	 */
	[[nodiscard]] int GetHeight() const
	{
		return height;
	}

	/**
	 * @brief Set texture pixel per unit
	 * @param value Pixel per unit
	 */
	void SetPixelPerUnit(int value)
	{
		if (value <= 0)
		{
			value = 1; // Ensure pixel per unit is always positive
		}
		m_settings.at(Application::GetAssetPlatform())->pixelPerUnit = value;
	}

	/**
	 * @brief Get texture pixel per unit
	 */
	[[nodiscard]] int GetPixelPerUnit() const
	{
		return m_settings.at(Application::GetAssetPlatform())->pixelPerUnit;
	}

	/**
	 * @brief Get if the texture is using mipmap
	 */
	[[nodiscard]] bool GetUseMipmap() const
	{
		return m_settings.at(Application::GetAssetPlatform())->useMipMap;
	}

	/**
	 * @brief Get texture filter
	 */
	[[nodiscard]] Filter GetFilter() const
	{
		return m_settings.at(Application::GetAssetPlatform())->filter;
	}
	
	/**
	 * @brief Get texture wrap mode
	 */
	[[nodiscard]] WrapMode GetWrapMode() const
	{
		return m_settings.at(Application::GetAssetPlatform())->wrapMode;
	}

	/**
	 * @brief Set texture data
	 * @param data Texture data
	 */
	virtual void SetData(const unsigned char* data) = 0;

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
	friend class EditorIcons;

	void SetSize(int width, int height)
	{
		this->m_width = width;
		this->height = height;
	}

	/**
	* @brief Get texture channel count
	*/
	[[nodiscard]] int GetChannelCount() const
	{
		return nrChannels;
	}

	void SetChannelCount(int channelCount)
	{
		this->nrChannels = channelCount;
	}

	/**
	 * @brief Return if the texture is valid
	 */
	[[nodiscard]] bool IsValid() const
	{
		return isValid;
	}

	/**
	* @brief [Internal] Get mipmap level count
	* @return 0 if mipmapping is not used
	*/
	/*[[nodiscard]] int GetMipmaplevelCount() const
	{
		return m_settings.at(Application::GetAssetPlatform())->mipmaplevelCount;
	}*/

	[[nodiscard]] TextureResolution GetCookResolution() const
	{
		return m_settings.at(Application::GetAssetPlatform())->resolution;
	}

	/**
	 * @brief [Internal]
	 */
	[[nodiscard]] std::shared_ptr<Texture> GetThisShared()
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

	static std::shared_ptr<Texture> MakeTexture(bool isDynamic = false);

	void LoadFileReference(const LoadOptions& loadOptions) override;
	void UnloadFileReference() override;

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
	int m_originalWidth = 0, m_originalHeight = 0;

#if defined(EDITOR)
	TextureResolution previousResolution = TextureResolution::R_2048x2048;
#endif
	bool isValid = false;
};
