#pragma once

#include <string>

// class Texture
// {
// public:
//     void Bind();
//     void Load(const char *filename, const int vram);
//     int width, height;

//     void SetData(const unsigned char *data, int vram, bool needResize);

// #ifdef __PSP__
//     unsigned int pW = 0;
//     unsigned int pH = 0;
//     void *data = nullptr;
//     int type;
// #endif

//     unsigned int id;
//     std::string name = "";
// };

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
    Texture(const std::string filePath, std::string name);
    Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap);
    Texture(const int textureId, const int channelCount, const int width, const int height);
    Texture(unsigned char *data, const int channelCount, const int width, const int height);

    ~Texture();

    void SetData(const unsigned char *data, int vram, bool needResize);
    // void Bind(); // To move?

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

#ifdef __PSP__
    void *data = nullptr;
    unsigned int pW = 0;
    unsigned int pH = 0;
    int type;
#endif

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
