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
std::vector<MeshData *> TextManager::meshes;
ProfilerBenchmark *textBenchmark = nullptr;

/**
 * @brief Init text manager
 *
 */
void TextManager::Init()
{
    meshes = std::vector<MeshData *>();
    textBenchmark = new ProfilerBenchmark("Text");
}

/**
 * @brief Draw mesh
 *
 * @param mesh Mesh data
 * @param for3D 3D text mode
 */
void TextManager::DrawTextMesh(MeshData *mesh, bool for3D, bool invertFaces)
{
    // Set draw settings
    RenderingSettings renderSettings = RenderingSettings();
    renderSettings.invertFaces = invertFaces;
    renderSettings.useBlend = true;
    renderSettings.useDepth = for3D;
    renderSettings.useTexture = true;

    Engine::renderer->DrawMeshData(mesh, renderSettings);
}

/**
 * @brief Delete texts meshes
 *
 */
void TextManager::ClearTexts()
{
    int meshCount = meshes.size();
    for (int meshIndex = 0; meshIndex < meshCount; meshIndex++)
    {
        delete meshes[meshIndex];
    }
    meshes.clear();
}

/**
 * @brief Set the text mesh position
 *
 * @param transform
 * @param canvas
 */
void TextManager::SetTextPosition(std::weak_ptr<Transform>transform, bool canvas)
{
    // Set text scale and pivot position/rotation
    Vector3 pos;
    if (!canvas)
    {
        pos = transform.lock()->GetPosition();
    }
    else
    {
        float xOff = (-Window::GetAspectRatio() * 5) + (transform.lock()->GetPosition().x * (Window::GetAspectRatio() * 10));
        float yOff = (-1 * 5) + (transform.lock()->GetPosition().y * (1 * 10));
        pos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane
    }

    Vector3 scl = transform.lock()->GetScale();
    scl.x = -scl.x;
    Engine::renderer->SetTransform(pos, transform.lock()->GetRotation(), scl, true);
}

/**
 * @brief Draw text
 *
 * @param text String to draw
 * @param horizontalAlignment Horizontal Alignment
 * @param verticalAlignment Vertical Alignment
 * @param transform Transform
 * @param canvas Is for canvas
 */
void TextManager::DrawText(std::string text, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, std::weak_ptr<Transform> transform, Color color, bool canvas)
{
    textBenchmark->Start();
    int textLenght = (int)text.size();

    if (textLenght == 0)
    {
        textBenchmark->Stop();
        return;
    }

    Font *font = fonts[0];

    // Get the size of the text
    TextInfo textInfo = GetTextInfomations(text, textLenght, font, 1);

    // Set text start offset
    float totalY = 0;
    for (int i = 0; i < textInfo.lineCount; i++)
    {
        totalY += textInfo.maxLineHeight;
    }

    float x = 0;
    float y = 0;
    int line = 0;
    if (horizontalAlignment == H_Left)
        x = -textInfo.linesInfo[line].lenght;
    else if (horizontalAlignment == H_Center)
        x = -textInfo.linesInfo[line].lenght * 0.5f;

    y = textInfo.linesInfo[line].y1 * 0.25f;
    y += -textInfo.maxLineHeight;

    if (verticalAlignment == V_Center)
    {
        y += totalY * 0.5f;
    }
    else if (verticalAlignment == V_Top)
    {
        y += totalY;
    }

    // Create empty mesh
    int charCountToDraw = textLenght - (textInfo.lineCount - 1);
    MeshData *mesh = new MeshData(4 * charCountToDraw, 6 * charCountToDraw, false);
    mesh->unifiedColor = color;
    meshes.push_back(mesh);

    int drawnCharIndex = 0;
    for (int i = 0; i < textLenght; i++)
    {
        char c = text[i];
        Character *ch = font->Characters[c];

        if (c == '\n')
        {
            line++;

            if (horizontalAlignment == H_Left)
                x = -textInfo.linesInfo[line].lenght;
            else if (horizontalAlignment == H_Center)
                x = -textInfo.linesInfo[line].lenght * 0.5f;
            else
                x = 0;

            y += -textInfo.maxLineHeight;
        }
        else
        {
            AddCharToMesh(mesh, ch, x, y, drawnCharIndex);
            drawnCharIndex++;
            x += ch->rightAdvance;
        }
    }

#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

    // Set projection
    if (!canvas)
    {
        Graphics::usedCamera->UpdateProjection();
        Engine::renderer->SetCameraPosition(Graphics::usedCamera);
    }
    else
    {
        Engine::renderer->SetProjection2D(5, 0.03f, 100);
        Engine::renderer->ResetView();
    }

    SetTextPosition(transform, canvas);

    Engine::renderer->BindTexture(font->fontAtlas);

    bool invertFaces = false;
    if (transform.lock()->GetScale().x * transform.lock()->GetScale().y < 0)
        invertFaces = true;

    DrawTextMesh(mesh, !canvas, invertFaces);
    textBenchmark->Stop();
}

/**
 * @brief Add a char to the mesh
 *
 * @param mesh Mesh to modify
 * @param ch Char to add
 * @param x Char X position
 * @param y Char Y position
 * @param letterIndex Letter index in the string
 */
void TextManager::AddCharToMesh(MeshData *mesh, Character *ch, float x, float y, int letterIndex)
{
    int indice = letterIndex * 4;
    int indiceIndex = letterIndex * 6;

    float w = ch->rightSize.x;
    float h = ch->rightSize.y;

    // unsigned int charColor = 0xFFFFFFFF;
    // Color charColor = Color::CreateFromRGBA(255, 0, 255, 255);

    float fixedY = y - (ch->rightSize.y - ch->rightBearing.y);

    mesh->AddVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, indice);
    mesh->AddVertex(ch->uvOffet.x, ch->uv.y, x, fixedY, 0, 1 + indice);
    mesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 2 + indice);
    mesh->AddVertex(ch->uv.x, ch->uvOffet.y, w + x, h + fixedY, 0, 3 + indice);

    mesh->indices[0 + indiceIndex] = 0 + indice;
    mesh->indices[1 + indiceIndex] = 2 + indice;
    mesh->indices[2 + indiceIndex] = 1 + indice;
    mesh->indices[3 + indiceIndex] = 2 + indice;
    mesh->indices[4 + indiceIndex] = 0 + indice;
    mesh->indices[5 + indiceIndex] = 3 + indice;
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

    int charPixelHeight = 48;
    // Load glyph
    FT_Set_Pixel_Sizes(face, 0, charPixelHeight);
    // Engine::renderer->PixelStoreUnpack();
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    int atlasSize = 512;
    int channelCount = 1;
#ifdef __PSP__
    channelCount = 4;
#endif

    unsigned char *atlas = (unsigned char *)calloc(atlasSize * atlasSize * channelCount, sizeof(unsigned char));

    int xOffset = 0;
    int yOffset = 0;

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

            // now store character for later use
            Character *character = new Character();
            character->Size = glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
            character->Bearing = glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top);

            character->rightSize = Vector2(face->glyph->bitmap.width * 0.01f, face->glyph->bitmap.rows * 0.01f);
            character->rightBearing = Vector2(face->glyph->bitmap_left * 0.01f, face->glyph->bitmap_top * 0.01f);

            character->Advance = (unsigned int)face->glyph->advance.x;

            character->rightAdvance = (face->glyph->advance.x >> 6) * 0.01f;

            font->Characters[c] = character;

            if (font->maxCharHeight < (float)character->rightSize.y)
                font->maxCharHeight = (float)character->rightSize.y;

            if (xOffset + face->glyph->bitmap.width >= atlasSize)
            {
                xOffset = 0;
                yOffset += charPixelHeight;
            }

            character->uvOffet = Vector2(xOffset / (float)atlasSize, yOffset / (float)atlasSize);
            character->uv = Vector2((xOffset + face->glyph->bitmap.width) / (float)atlasSize, (yOffset + face->glyph->bitmap.rows) / (float)atlasSize);

            if (c >= 32) // Do not render invisible chars
            {
                int textureXOffset = xOffset * channelCount;
                for (int fW = 0; fW < face->glyph->bitmap.rows; fW++)
                {
                    for (int fH = 0; fH < face->glyph->bitmap.width; fH++)
                    {
                        int atlasOffset = (fH * channelCount) + (fW * atlasSize * channelCount) + textureXOffset + yOffset * atlasSize * channelCount;
#ifdef __PSP__
                        atlas[atlasOffset] = 255;
                        atlas[atlasOffset + 1] = 255;
                        atlas[atlasOffset + 2] = 255;
                        atlas[atlasOffset + 3] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
#else
                        atlas[atlasOffset] = face->glyph->bitmap.buffer[fH + (fW * face->glyph->bitmap.width)];
#endif
                    }
                }
                xOffset += face->glyph->bitmap.width + 1;
            }
        }
        catch (...)
        {
            Debug::Print("ERROR: Failed to load Glyph. Path: " + path);
            return nullptr;
        }
    }

    font->fontAtlas = new Texture(atlas, channelCount, atlasSize, atlasSize, false);
    font->fontAtlas->SetFilter(Texture::Bilinear);
    font->fontAtlas->SetWrapMode(Texture::ClampToEdge);

    Debug::Print("Font loaded");

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif

    fonts.push_back(font);
    return font;
}

/**
 * @brief Get informations about the text
 *
 * @param text Text to analyse
 * @param textLen Lenght of the text
 * @param font Used font
 * @param scale Scale of the text
 * @return TextInfo
 */
TextInfo TextManager::GetTextInfomations(std::string &text, int textLen, Font *font, float scale)
{
    TextInfo textInfos = TextInfo();
    textInfos.linesInfo.push_back(LineInfo());

    int currentLine = 0;
    float higherY = 0;
    float lowerY = 0;

    for (int i = 0; i < textLen; i++)
    {
        Character *ch = font->Characters[text[i]];
        if (text[i] == '\n')
        {
            textInfos.linesInfo[currentLine].lenght *= scale;
            textInfos.linesInfo[currentLine].y1 = (higherY - lowerY) * scale;
            textInfos.linesInfo.push_back(LineInfo());
            currentLine++;
            higherY = 0;
            lowerY = 0;
        }
        else
        {
            textInfos.linesInfo[currentLine].lenght += ch->rightAdvance;
            if (higherY < ch->rightBearing.y)
                higherY = ch->rightBearing.y;

            float low = ch->rightSize.y - ch->rightBearing.y;
            if (lowerY < low)
                lowerY = low;
        }
    }
    textInfos.linesInfo[currentLine].lenght *= scale;
    textInfos.linesInfo[currentLine].y1 = (higherY - lowerY) * scale;

    textInfos.maxLineHeight = font->maxCharHeight * scale;
    textInfos.lineCount = currentLine + 1;

    return textInfos;
}