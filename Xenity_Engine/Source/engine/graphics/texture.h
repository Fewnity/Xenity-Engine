#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once

#include <string>
#include <vector>
#include "../file_system/file_reference.h"
#include "../reflection/reflection.h"

class API Texture : public FileReference, public Reflection
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
        ClampToEdge,
        ClampToBorder,
        MirroredRepeat,
        Repeat,
        MirrorClampToEdge
    };

    Texture();

    /**
    * [Internal]
    */
    Texture(unsigned char *data, const int channelCount, const int width, const int height, bool loadInVram);

    std::unordered_map<std::string, ReflectionEntry> GetReflection() override;
    std::unordered_map<std::string, ReflectionEntry> GetMetaReflection() override;

    static std::shared_ptr<Texture> MakeTexture();
    static std::shared_ptr<Texture> MakeTexture(unsigned char* data, const int channelCount, const int width, const int height, bool loadInVram);

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
    * Get texture ID
    */
    unsigned int GetTextureId() const;

    /**
    * Update texture if a filter changed
    */
    void UpdateTextureFilter();

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

    /**
    * Get if the texture is using mipmap
    */
    bool GetUseMipmap() const;

    /**
    * Get texture filter
    */
    Texture::Filter GetFilter() const;

    /**
    * Get texture wrap mode
    */
    Texture::WrapMode GetWrapMode() const;

    int mipmaplevelCount = 0;
    bool useMipMap = false;
    bool inVram = true;

    /**
    * Return if the texture is valid
    */
    bool IsValid()
    {
        return isValid;
    }

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
    unsigned int pW = 0;
    unsigned int pH = 0;
    int type;
#endif

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

    Filter filter = Bilinear;
    WrapMode wrapMode = Repeat;
    unsigned int textureId = 0;
    int width = 0, height = 0, nrChannels = 0;

    int pixelPerUnit = 100;
    bool isValid = false;
    unsigned char* buffer = nullptr;
};
