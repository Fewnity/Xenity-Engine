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
    Texture(const std::string filePath, std::string name, bool loadInVram);
    Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap, bool loadInVram);
    Texture(const int textureId, const int channelCount, const int width, const int height, bool loadInVram);
    Texture(unsigned char *data, const int channelCount, const int width, const int height, bool loadInVram);
    std::unordered_map<std::string, Variable> GetReflection();
    std::unordered_map<std::string, Variable> GetMetaReflection();

    ~Texture();

    void LoadFileReference();
    void UnloadFileReference();

    void SetData(const unsigned char *data);

    unsigned int GetTextureId() const;
    void UpdateTextureFilter();
    void SetFilter(const Filter filter);
    void SetWrapMode(const WrapMode mode);
    int GetWidth() const;
    int GetHeight() const;
    void SetPixelPerUnit(int value);
    int GetPixelPerUnit() const;
    int GetChannelCount() const;
    bool GetUseMipmap() const;
    Texture::Filter GetFilter() const;
    Texture::WrapMode GetWrapMode() const;

    std::string name = "";
    int mipmaplevelCount = 0;
    bool useMipMap = false;
    bool inVram = true;

    bool IsValid()
    {
        return isValid;
    };

#ifdef __PSP__
    void SetTextureLevel(int level, const unsigned char *texData);

    std::vector<void *> data;
    unsigned int pW = 0;
    unsigned int pH = 0;
    int type;
#endif

private:
    Filter filter = Bilinear;
    WrapMode wrapMode = Repeat;
    void CreateTexture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap);
    void LoadTexture(const std::string filePath);
    void Unload();
    unsigned int textureId = -1;
    int width = 0, height = 0, nrChannels = 0;

    int pixelPerUnit = 100;
    bool isValid = false;
};
