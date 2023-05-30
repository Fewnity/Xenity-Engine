#include "text_manager.h"
#include "../../../xenity.h"

#ifdef __PSP__
#include <pspkernel.h>
#include "../../../psp/gu2gl.h"
#include "../../../psp/graphics/graphics.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#include "../../../psvita/graphics/graphics.h"
#endif

#include <ft2build.h>

// #include <freetype/ft2build.h>

#include FT_FREETYPE_H


std::vector<Font*> TextManager::fonts;

unsigned int pow22(const unsigned int value)
{
    unsigned int poweroftwo = 1;
    while (poweroftwo < value)
    {
        poweroftwo <<= 1;
    }
    return poweroftwo;
}

/// <summary>
/// Load a font
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
Font* TextManager::CreateFont(std::string filePath)
{
    //return nullptr;
    Debug::Print("Loading font...");

    Font* font = new Font();

    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        Debug::Print("ERROR: Could not init FreeType Library");
        return nullptr;
    }

    // Load font
    FT_Face face;
    // std::string path = EngineSettings::RootFolder + filePath;
    std::string path;
#ifdef __vita__
    path = "ux0:";
#endif
    path += EngineSettings::RootFolder + filePath;
    if (FT_New_Face(ft, path.c_str(), 0, &face))
    {
        Debug::Print("ERROR: Failed to load font. Path: " + path);
        return nullptr;
    }

    // 34, 64

    // Load glyph
    FT_Set_Pixel_Sizes(face, 0, 48);
    // Engine::renderer->PixelStoreUnpack();
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 100; c++)
    {
        try
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            {
                Debug::Print("ERROR: Failed to load Glyph. Path: " + path);
                continue;
            }
            Texture* texture = new Texture();

#ifdef __PSP__
            int pW = pow22(face->glyph->bitmap.width);
            int pH = pow22(face->glyph->bitmap.rows);

            unsigned char* test = (unsigned char*)malloc(sizeof(unsigned char) * pW * pH * 4);
            if (face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
            {
                // Debug::Print("a " +  std::to_string(c) + " " + std::to_string(face->glyph->bitmap.width) + ", " + std::to_string(face->glyph->bitmap.rows));
                unsigned char* texData2 = (unsigned char*)malloc(pW * pH * 1);
                stbir_resize_uint8(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, texData2, pW, pH, 0, 1);
                int size = pW * pH;
                for (int i = 0; i < size; i++)
                {
                    int off = size - i - 1;
                    test[0 + i * 4] = texData2[off];
                    test[1 + i * 4] = texData2[off];
                    test[2 + i * 4] = texData2[off];
                    test[3 + i * 4] = texData2[off];
                }
                free(texData2);
            }
#endif

            texture->width = face->glyph->bitmap.width;
            texture->height = face->glyph->bitmap.rows;

#ifdef __PSP__
            texture->SetData(test, 0);
            free(test);
            sceKernelDcacheWritebackInvalidateAll();
#else
            texture->SetData(face->glyph->bitmap.buffer, 0);
#endif

            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int)face->glyph->advance.x };
            font->Characters[c] = character;

            if (font->maxCharHeight < (float)character.Size.y)
                font->maxCharHeight = (float)character.Size.y;

            // Debug::Print("b " +  std::to_string(c) + " " + std::to_string(texture->pW) + ", " + std::to_string(texture->pH));
        }
        catch (...)
        {
            Debug::Print("ERROR: Failed to load Glyph. Path: " + path);
            return nullptr;
        }
    }
    Debug::Print("Font loaded");

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    fonts.push_back(font);
    return font;
}