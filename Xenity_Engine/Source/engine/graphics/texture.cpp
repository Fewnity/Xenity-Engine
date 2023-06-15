#include "texture.h"
#include "../../xenity.h"

#include "renderer/renderer.h"

#include "../../../include/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../../include/stb_image_resize.h"
#include <malloc.h>
#include <string>

#ifdef __vita__
#include <vitaGL.h>
#endif

#ifdef __PSP__
#include "../../psp/gu2gl.h"
#include <pspkernel.h>
#include <vram.h>
#endif

Texture::Texture(const std::string filePath, std::string name, bool loadInVram)
{
    this->inVram = loadInVram;
    CreateTexture(filePath, name, Bilinear, false);
}

Texture::Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap, bool loadInVram)
{
    this->inVram = loadInVram;
    CreateTexture(filePath, name, filter, useMipMap);
}

Texture::Texture(const int textureId, const int channelCount, const int width, const int height, bool loadInVram)
{
    this->textureId = textureId;
    this->nrChannels = channelCount;
    this->width = width;
    this->height = height;
    this->inVram = loadInVram;
    useMipMap = false;
}

Texture::Texture(unsigned char *data, const int channelCount, const int width, const int height, bool loadInVram)
{
    // this->textureId = textureId;
    this->nrChannels = channelCount;
    this->width = width;
    this->height = height;
    this->inVram = loadInVram;

    useMipMap = false;
    SetData(data);
}

Texture::~Texture()
{
    AssetManager::RemoveTexture(this);
    // Engine::renderer->DeleteTexture(this);
}

/// <summary>
/// Create the texture from the file path and texture settings
/// </summary>
/// <param name="filePath">File path</param>
/// <param name="filter">Filter to use</param>
/// <param name="useMipMap">Will texture use mipmap</param>
void Texture::CreateTexture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap)
{
    this->filter = filter;
    this->useMipMap = useMipMap;
    this->name = name;

    LoadTexture(filePath);

    AssetManager::AddTexture(this);
}

#if defined(__PSP__)
void swizzle_fast(u8 *out, const u8 *in, const unsigned int width, const unsigned int height)
{
    unsigned int blockx, blocky;
    unsigned int j;

    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    const u8 *ysrc = in;
    u32 *dst = (u32 *)out;

    for (blocky = 0; blocky < height_blocks; ++blocky)
    {
        const u8 *xsrc = ysrc;
        for (blockx = 0; blockx < width_blocks; ++blockx)
        {
            const u32 *src = (u32 *)xsrc;
            for (j = 0; j < 8; ++j)
            {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

void copy_texture_data(void *dest, const void *src, const int pW, const int width, const int height)
{
    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            ((unsigned int *)dest)[x + y * pW] = ((unsigned int *)src)[x + y * width];
        }
    }
}

void Texture::SetTextureLevel(int level, const unsigned char *texData)
{
    bool needResize = false;
    int bytePerPixel = 4;
    int diviser = (int)pow(2, level);

    int resizedPW = pW / diviser;
    int resizedPH = pH / diviser;

    int byteCount = (resizedPW * resizedPH) * bytePerPixel;

    unsigned int *dataBuffer = (unsigned int *)memalign(16, byteCount);

    if (level != 0 || (width != pW || height != pH))
    {
        needResize = true;
    }

    if (needResize)
    {
        unsigned char *resizedData = (unsigned char *)malloc(byteCount);
        stbir_resize_uint8(texData, width, height, 0, resizedData, resizedPW, resizedPH, 0, bytePerPixel);
        copy_texture_data(dataBuffer, resizedData, resizedPW, resizedPW, resizedPH);
        free(resizedData);
    }
    else
    {
        copy_texture_data(dataBuffer, texData, pW, pW, pH);
    }

    if (resizedPW > 256 || resizedPH > 256)
        inVram = false;

    // Allocate memory in ram or vram
    if (inVram)
    {
        unsigned int *newData = (unsigned int *)vramalloc(byteCount);
        // If there is no more free vram
        if (!newData)
        {
            Debug::PrintWarning("No more free vram");
            newData = (unsigned int *)memalign(16, byteCount);
            inVram = false;
        }
        data.push_back((unsigned int *)newData);
    }
    else
    {
        data.push_back((unsigned int *)memalign(16, byteCount));
    }

    // Place image data in the memory
    swizzle_fast((u8 *)data[level], (const u8 *)dataBuffer, resizedPW * bytePerPixel, resizedPH);
    free(dataBuffer);
    sceKernelDcacheWritebackInvalidateAll();
}

#endif

void Texture::SetData(const unsigned char *texData)
{
    // sceGeEdramSetSize(4096);
    // The psp needs a pow2 sized texture
#if defined(__PSP__)
    type = GU_PSM_8888;

    // Get pow2 size
    pW = Math::pow2(width);
    pH = Math::pow2(height);

    SetTextureLevel(0, texData);
    if (useMipMap)
    {
        SetTextureLevel(1, texData);
        // SetTextureLevel(2, texData);
        // SetTextureLevel(3, texData);
        mipmaplevelCount = 1;
    }

#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
    //glGenTextures(1, &textureId);
    //glBindTexture(GL_TEXTURE_2D, textureId);
    textureId = Engine::renderer->CreateNewTexture();
    Engine::renderer->BindTexture(this);

    unsigned int alpha = 0x1906;
    Engine::renderer->SetTextureData(this, alpha, texData);

    //glTexImage2D(GL_TEXTURE_2D, 0, alpha, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texData);
    //if (useMipMap)
        //glGenerateMipmap(GL_TEXTURE_2D);
#endif
}

void Texture::LoadTexture(const std::string filename)
{
    std::string path = "";
#if defined(__vita__)
    path += "ux0:";
#endif
    path += filename;

    std::string debugText = "Loading texture: ";
    debugText += filename;
    Debug::Print(debugText);

    // Load image with stb_image
    // stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *buffer = stbi_load(path.c_str(), &width, &height,
                                      &nrChannels, 4);
    if (!buffer)
    {
        debugText = "Failed to load texture: ";
        debugText += filename;
        Debug::Print(debugText);
        return;
    }
#if defined(__PSP__)
    SetData(buffer);
#endif

#if defined(__vita__) || defined(_WIN32) || defined(_WIN64)
   // glGenTextures(1, &textureId);
    textureId = Engine::renderer->CreateNewTexture();
    Engine::renderer->BindTexture(this);
    //glBindTexture(GL_TEXTURE_2D, textureId);

    unsigned int rgba = 0x1908;
    Engine::renderer->SetTextureData(this, rgba, buffer);

    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
    //if (useMipMap)
        //glGenerateMipmap(GL_TEXTURE_2D);
#endif

    stbi_image_free(buffer);
}

/// <summary>
/// Set texture's filter
/// </summary>
/// <param name="filter"></param>
void Texture::SetFilter(const Filter filter)
{
    this->filter = filter;
    UpdateTextureFilter();
}

void Texture::SetWrapMode(const WrapMode mode)
{
    wrapMode = mode;
    UpdateTextureFilter();
}

int Texture::GetWidth() const
{
    return width;
}

int Texture::GetHeight() const
{
    return height;
}

void Texture::SetPixelPerUnit(int value)
{
    pixelPerUnit = value;
}

int Texture::GetPixelPerUnit() const
{
    return pixelPerUnit;
}

int Texture::GetChannelCount() const
{
    return nrChannels;
}

bool Texture::GetUseMipmap() const
{
    return useMipMap;
}

/// <summary>
/// Get texture id
/// </summary>
/// <returns></returns>
unsigned int Texture::GetTextureId() const
{
    return textureId;
}

Texture::Filter Texture::GetFilter() const
{
    return filter;
}

Texture::WrapMode Texture::GetWrapMode() const
{
    return wrapMode;
}

#pragma endregion

/// <summary>
/// Update texture if the filter has changed
/// </summary>
void Texture::UpdateTextureFilter()
{
    // Engine::renderer->BindTexture(this);
    // Engine::renderer->SetTextureFilter(this, filter);
    // Engine::renderer->SetTextureWrapMode(wrapMode);
    // Engine::renderer->SetAnisotropyLevel(EngineSettings::anisotropicLevel);
}