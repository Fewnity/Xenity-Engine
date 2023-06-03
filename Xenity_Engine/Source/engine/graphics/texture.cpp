#include "texture.h"
#include "../../xenity.h"

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
#endif

Texture::Texture(const std::string filePath, std::string name)
{
    CreateTexture(filePath, name, Bilinear, true);
}

Texture::Texture(const std::string filePath, std::string name, const Filter filter, const bool useMipMap)
{
    CreateTexture(filePath, name, filter, useMipMap);
}

Texture::Texture(const int textureId, const int channelCount, const int width, const int height)
{
    this->textureId = textureId;
    this->nrChannels = channelCount;
    this->width = width;
    this->height = height;
    useMipMap = false;
}

Texture::Texture(unsigned char *data, const int channelCount, const int width, const int height)
{
    // this->textureId = textureId;
    this->nrChannels = channelCount;
    this->width = width;
    this->height = height;
    useMipMap = false;
    SetData(data, 0, false);
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

#ifdef __PSP__
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

#endif

void Texture::SetData(const unsigned char *texData, int vram, bool needResize)
{
#ifdef __PSP__
    type = GU_PSM_8888;
    int bytePerPixel = 4;

    pW = Math::pow2(width);
    pH = Math::pow2(height);
    int byteCount = pW * pH * bytePerPixel;

    unsigned int *dataBuffer = (unsigned int *)memalign(16, byteCount);
    if (needResize)
    {
        unsigned char *resizedData = (unsigned char *)malloc(byteCount);
        stbir_resize_uint8(texData, width, height, 0, resizedData, pW, pH, 0, bytePerPixel);
        copy_texture_data(dataBuffer, resizedData, pW, pW, pH);
    }
    else
    {
        // Copy to Data Buffer
        copy_texture_data(dataBuffer, texData, pW, pW, pH);
    }

    unsigned int *swizzled_pixels = NULL;
    if (vram)
    {
        swizzled_pixels = (unsigned int *)getStaticVramTexture(pW, pH, type);
    }
    else
    {
        swizzled_pixels = (unsigned int *)memalign(16, byteCount);
    }

    swizzle_fast((u8 *)swizzled_pixels, (const u8 *)dataBuffer, pW * bytePerPixel, pH);
    free(dataBuffer);

    data = swizzled_pixels;

    sceKernelDcacheWritebackInvalidateAll();

#endif

#ifdef __vita__
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, texData);
#endif
}

// void Texture::Bind()
// {
// #ifdef __PSP__
//     glTexMode(type, 0, 0, 1);
//     glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA);
//     glTexFilter(GL_NEAREST, GL_NEAREST);
//     glTexWrap(GL_REPEAT, GL_REPEAT);
//     glTexImage(0, pW, pH, pW, data);
// #endif
// #ifdef __vita__
//     glBindTexture(GL_TEXTURE_2D, textureId);
// #endif
// }

void Texture::LoadTexture(const std::string filename)
{
    std::string path = "";
#ifdef __vita__
    path += "ux0:";
#endif
    path += filename;

    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *buffer = stbi_load(path.c_str(), &width, &height,
                                      &nrChannels, 4);
    if (!buffer)
    {
        std::string t = "Failed to load texture: ";
        t += filename;
        Debug::Print(t);
        return;
    }
    else
    {
        Debug::Print("Texture Found");
    }

#ifdef __PSP__
    int vram = 0;
    SetData(buffer, vram, true);
#endif

#ifdef __vita__
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
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