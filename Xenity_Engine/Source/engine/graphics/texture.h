#pragma once

#include <string>
#include <vector>

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
        ClampToEdge,
        ClampToBorder,
        MirroredRepeat,
        Repeat,
        MirrorClampToEdge
    };

    Texture() = delete;
    Texture(const std::string filePath, std::string name, bool loadInVram);
    Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap, bool loadInVram);
    Texture(const int textureId, const int channelCount, const int width, const int height, bool loadInVram);
    Texture(unsigned char *data, const int channelCount, const int width, const int height, bool loadInVram);

    ~Texture();

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

#ifdef __PSP__
    void SetTextureLevel(int level, const unsigned char *texData);

    std::vector<void *> data;
    // void *data = nullptr;
    // void *data2 = nullptr;
    unsigned int pW = 0;
    unsigned int pH = 0;
    int type;
#endif

private:
    Filter filter = Bilinear;
    WrapMode wrapMode = Repeat;
    void CreateTexture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap);
    void LoadTexture(const std::string filePath);
    unsigned int textureId;
    int width = 0, height = 0, nrChannels = 0;

    int pixelPerUnit = 100;
};
