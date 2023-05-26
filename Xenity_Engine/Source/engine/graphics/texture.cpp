#include "texture.h"

#include "../../../include/stb_image.h"
#include <malloc.h>
#include <string>
#include "../debug/debug.h"

#ifdef __PSP__
#include "../../psp/gu2gl.h"
#include <pspkernel.h>

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

unsigned int pow2(const unsigned int value)
{
    unsigned int poweroftwo = 1;
    while (poweroftwo < value)
    {
        poweroftwo <<= 1;
    }
    return poweroftwo;
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

void Texture::Bind()
{
    // Specifics for psp
    glTexMode(GU_PSM_8888, 0, 0, 1);
    glTexFunc(GL_TFX_MODULATE, GL_TCC_RGBA);
    glTexFilter(GL_NEAREST, GL_NEAREST);
    glTexWrap(GL_REPEAT, GL_REPEAT);
    glTexImage(0, pW, pH, pW, data);
}

void Texture::Load(const char *filename, const int vram)
{
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *texData = stbi_load(filename, &width, &height,
                                       &nrChannels, STBI_rgb_alpha);

    if (!texData)
    {
        std::string t = "Failed to load texture: ";
        t += filename;
        Debug::Print(t);
        return;
    }

    width = width;
    height = height;
    pW = pow2(width);
    pH = pow2(height);

    unsigned int *dataBuffer =
        (unsigned int *)memalign(16, pH * pW * 4);

    // Copy to Data Buffer
    copy_texture_data(dataBuffer, texData, pW, width, height);

    // Free STB Data
    stbi_image_free(texData);

    unsigned int *swizzled_pixels = NULL;
    size_t size = pH * pW * 4;
    if (vram)
    {
        swizzled_pixels = (unsigned int *)getStaticVramTexture(pW, pH, GU_PSM_8888);
    }
    else
    {
        swizzled_pixels = (unsigned int *)memalign(16, size);
    }

    swizzle_fast((u8 *)swizzled_pixels, (const u8 *)dataBuffer, pW * 4, pH);

    free(dataBuffer);
    data = swizzled_pixels;

    sceKernelDcacheWritebackInvalidateAll();

    // return tex;
}

#endif

#ifdef __vita__

#include <vitaGL.h>

void Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Load(const char *filename, const int vram)
{
    int nrChannels;
    // unsigned char *buffer = stbi_load("app0:texture.bmp", &width, &height,
    //                                   &nrChannels, 3);
    stbi_set_flip_vertically_on_load(GL_TRUE);
    unsigned char *buffer = stbi_load("ux0:container.jpg", &width, &height,
                                      &nrChannels, 3);
    if (!buffer)
    {
        Debug::Print("No Texture Found");
    }
    else
    {
        Debug::Print("Texture Found");
    }
    glGenTextures(1, &id);
    Bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
}

#endif