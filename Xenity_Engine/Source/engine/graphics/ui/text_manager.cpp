#include "text_manager.h"
#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"

#include "../renderer/renderer.h"

#ifdef __PSP__
#include <pspkernel.h>
#include "../../../psp/gu2gl.h"
#endif

#ifdef __vita__
#include <vitaGL.h>
#endif

#include <ft2build.h>
#include "../../../../include/stb_image_resize.h"

#include FT_FREETYPE_H

std::vector<Font *> TextManager::fonts;

// MeshData *TextManager::spriteMeshData = nullptr;

// Texture *ttexture;

void TextManager::Init()
{
    // ttexture = new Texture();
    // ttexture->Load("container.jpg", 0);
    //     spriteMeshData = new MeshData(4, 6);
    //     spriteMeshData->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f, -0.5f, 0.0f, 0);
    //     spriteMeshData->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f, -0.5f, 0.0f, 1);
    //     spriteMeshData->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f, 0.5f, 0.0f, 2);
    //     spriteMeshData->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f, 0.5f, 0.0f, 3);
    //     spriteMeshData->indices[0] = 0;
    //     spriteMeshData->indices[1] = 2;
    //     spriteMeshData->indices[2] = 1;
    //     spriteMeshData->indices[3] = 2;
    //     spriteMeshData->indices[4] = 0;
    //     spriteMeshData->indices[5] = 3;
    // #ifdef __PSP__
    //     sceKernelDcacheWritebackInvalidateAll(); // Very important
    // #endif
}

void TextManager::CreateCharacterMesh(Character *chara)
{
    // if (spriteMeshData)
    //     delete spriteMeshData;

    float scaleCoef = (100.0f / 100.0f) / 100.0f;
    float w = chara->texture->width * scaleCoef;
    float h = chara->texture->height * scaleCoef;

    chara->mesh = new MeshData(4, 6);
    // spriteMeshData->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f * w, -0.5f * h, 0.0f, 0);
    // spriteMeshData->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f * w, -0.5f * h, 0.0f, 1);
    // spriteMeshData->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f * w, 0.5f * h, 0.0f, 2);
    // spriteMeshData->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f * w, 0.5f * h, 0.0f, 3);

    chara->mesh->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, 0, 0, 0.0f, 0);
    chara->mesh->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 1 * w, 0, 0.0f, 1);
    chara->mesh->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 1 * w, 1 * h, 0.0f, 2);
    chara->mesh->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, 0 * w, 1 * h, 0.0f, 3);

    chara->mesh->indices[0] = 0;
    chara->mesh->indices[1] = 2;
    chara->mesh->indices[2] = 1;
    chara->mesh->indices[3] = 2;
    chara->mesh->indices[4] = 0;
    chara->mesh->indices[5] = 3;
#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
}

void TextManager::DrawCharacter(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture, Character *ch)
{
    // texture = TextManager::fonts[0]->Characters[65].texture;
    // float scaleCoef = 100.0f / texture->GetPixelPerUnit() / 100.0f;
    // float scaleCoef = (1.0f / 100.0f); //?

    float scaleCoef = (100.0f / 100.0f) / 100.0f;
    float w = texture->width * scaleCoef;
    float h = texture->height * scaleCoef;

    // Engine::renderer->SetCameraPosition(Graphics::usedCamera);

    Vector3 pos = Vector3(position.x, position.y, 0);
    // Vector3 rot = Vector3(rotation.x, rotation.y + 180, rotation.z);
    Vector3 rot = Vector3(0, 0, 0);
    // Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
    Vector3 scl = Vector3(1, 1, 1);
    Engine::renderer->SetTransform(pos, rot, scl, false);

    // Set settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    texture->Bind();
    // ttexture->Bind();
    MeshManager::DrawMeshData(ch->mesh);
}

void TextManager::DrawText(std::string text, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Transform *transform)
{
    Font *font = fonts[0];
    int textLenght = (int)text.size();

    float x = 0;
    float y = 0;

    std::vector<Vector4> lineLength = GetTextLenght(text, textLenght, font, 1);

    Engine::renderer->SetCameraPosition(Graphics::usedCamera);
    Vector3 pos = Vector3(0, 0, 0);
    Vector3 rot = Vector3(transform->GetRotation().x, transform->GetRotation().y + 180, transform->GetRotation().z);
    Vector3 scl = Vector3(1, 1, 1);
    Engine::renderer->SetTransform(pos, rot, scl, true);

    Vector2 nextMove = Vector2(0);

    float totalY = 0;
    int lineCount = (int)lineLength.size();
    for (int i = 0; i < lineCount; i++)
    {
        totalY += lineLength[i].z;
    }

    int line = 0;
    if (horizontalAlignment == H_Left)
        nextMove.x = -lineLength[line].x / 100.0f;
    else if (horizontalAlignment == H_Center)
        nextMove.x = -lineLength[line].x / 100.0f / 2.0f;

    nextMove.y = lineLength[line].y / 100.0f / 4.0f;
    nextMove.y += -lineLength[line].z / 100.0f;

    if (verticalAlignment == V_Center)
    {
        nextMove.y += totalY / 100.0f / 2.0f;
    }
    else if (verticalAlignment == V_Top)
    {
        nextMove.y += totalY / 100.0f;
    }

    for (int i = 0; i < textLenght; i++)
    {
        char c = text[i];
        Character *ch = &font->Characters[c];
        if (c == '\n')
        {
            line++;
            nextMove.x = -x;
            if (horizontalAlignment == H_Left)
                nextMove.x -= lineLength[line].x / 100.0f;
            else if (horizontalAlignment == H_Center)
                nextMove.x -= lineLength[line].x / 100.0f / 2.0f;

            nextMove.y = -lineLength[line].z / 100.0f;
            // nextMove.y = -0.5;
        }
        else
        {
            float yOff = (ch->Size.y - ch->Bearing.y) / 100.0f;
            // TextManager::CreateCharacterMesh(ch);
            DrawCharacter(Vector3(nextMove.x, nextMove.y - yOff, 0), Vector3(0, 0, 0), Vector3(1, 1, 1), ch->texture, ch);
            x += nextMove.x;
            nextMove.x = ((ch->Advance >> 6)) / 100.0f; // bitshift by 6 to get value in pixels (2^6 = 64)
            nextMove.y = yOff;
        }
    }
}

/// <summary>
/// Load a font
/// </summary>
/// <param name="filePath"></param>
/// <returns></returns>
Font *TextManager::CreateFont(std::string filePath)
{
    // return nullptr;
    Debug::Print("Loading font...");

    Font *font = new Font();

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

    // Load glyph
    FT_Set_Pixel_Sizes(face, 0, 48);
    // Engine::renderer->PixelStoreUnpack();
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 255; c++)
    {
        try
        {
            // load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            {
                Debug::Print("ERROR: Failed to load Glyph. Path: " + path);
                continue;
            }
            Texture *texture = new Texture();

#ifdef __PSP__
            int pW = Math::pow2(face->glyph->bitmap.width);
            int pH = Math::pow2(face->glyph->bitmap.rows);

            unsigned char *test = (unsigned char *)malloc(sizeof(unsigned char) * pW * pH * 4);
            if (face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
            {
                // Debug::Print("a " +  std::to_string(c) + " " + std::to_string(face->glyph->bitmap.width) + ", " + std::to_string(face->glyph->bitmap.rows));
                unsigned char *texData2 = (unsigned char *)malloc(pW * pH * 1);
                stbir_resize_uint8(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, texData2, pW, pH, 0, 1);

                int size = pW * pH;
                for (int i = 0; i < size; i++)
                {
                    test[0 + i * 4] = texData2[i];
                    test[1 + i * 4] = texData2[i];
                    test[2 + i * 4] = texData2[i];
                    test[3 + i * 4] = texData2[i];
                }
                free(texData2);
            }
#endif

            texture->width = face->glyph->bitmap.width;
            texture->height = face->glyph->bitmap.rows;

#ifdef __PSP__
            texture->SetData(test, 0, false);
            free(test);
            sceKernelDcacheWritebackInvalidateAll();
#else
            texture->SetData(face->glyph->bitmap.buffer, 0, false);
#endif

            // now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int)face->glyph->advance.x};
            TextManager::CreateCharacterMesh(&character);
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

std::vector<Vector4> TextManager::GetTextLenght(std::string &text, int textLen, Font *font, float scale)
{
    std::vector<Vector4> lineLength;
    lineLength.push_back(Vector4(0, 0, 0, 0));

    int currentLine = 0;
    int higherY = 0;
    int lowerY = 0;

    for (int i = 0; i < textLen; i++)
    {
        Character ch = font->Characters[text[i]];
        if (text[i] == '\n')
        {
            lineLength[currentLine].y = (higherY - lowerY) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
            lineLength[currentLine].z = font->maxCharHeight * scale;
            lineLength[currentLine].x *= scale;
            lineLength.push_back(Vector4(0, 0, 0, 0));
            currentLine++;
            higherY = 0;
            lowerY = 0;
        }
        else
        {
            lineLength[currentLine].x += (ch.Advance >> 6);
            if (higherY < (int)ch.Bearing.y)
                higherY = (int)ch.Bearing.y;

            int low = ch.Size.y - ch.Bearing.y;
            if (lowerY < low)
                lowerY = low;
        }
    }
    lineLength[currentLine].x *= scale;
    lineLength[currentLine].y = (higherY - lowerY) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    lineLength[currentLine].z = font->maxCharHeight * scale;
    return lineLength;
}