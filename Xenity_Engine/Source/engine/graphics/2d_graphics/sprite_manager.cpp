#include "sprite_manager.h"
#include "../../../xenity.h"
#include "../texture.h"
#include <cstdlib>
#include <malloc.h>
#include "../camera.h"
#include "../renderer/renderer.h"
#include "../../../../include/stb_image_resize.h"

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
// #include FT_CACHE_H
#include FT_FREETYPE_H

MeshData::MeshData(unsigned int vcount, unsigned int index_count)
{
    // data = (Vertex *)memalign(16, sizeof(Vertex) * vcount);
    data = (Vertex *)malloc(sizeof(Vertex) * vcount);
    if (data == nullptr)
    {
        return;
    }

    indices = (unsigned int *)malloc(sizeof(unsigned int) * index_count);
    // indices = (unsigned short *)malloc(sizeof(unsigned short) * index_count);

    // indices = (unsigned int *)memalign(16, sizeof(unsigned int) * index_count);
    // indices = (unsigned short *)memalign(16, sizeof(unsigned short) * index_count);
    //  mesh->indices = memalign(16, sizeof(u16) * index_count);
    if (indices == nullptr)
    {
        return;
    }

    this->index_count = index_count;
}

void MeshData::AddVertice(float u, float v, unsigned int color, float x, float y, float z, int indice)
{
    Vertex vert = {
        .u = u,
        .v = v,
        .color = color,
        .x = x,
        .y = y,
        .z = z};

    data[indice] = vert;
}

MeshData::~MeshData()
{
    if (data)
        free(data);
    if (indices)
        free(indices);
}

void DrawMeshData(MeshData *meshData)
{
#ifdef __PSP__
    // glDrawElements(GL_TRIANGLES, GL_INDEX_16BIT | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 6, meshData->indices, meshData->data);
    glDrawElements(GL_TRIANGLES, GL_INDEX_BITS | GL_TEXTURE_32BITF | GL_COLOR_8888 | GL_VERTEX_32BITF | GL_TRANSFORM_3D, 6, meshData->indices, meshData->data);
#endif

#ifdef __vita__
    glEnableClientState(GL_VERTEX_ARRAY);
    // glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    int stride = sizeof(Vertex);
    glTexCoordPointer(2, GL_FLOAT, stride, &meshData->data[0].u);
    // glColorPointer(1, GL_UNSIGNED_INT, stride, &vData->data[0].color);
    glVertexPointer(3, GL_FLOAT, stride, &meshData->data[0].x);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, meshData->indices);
    glDisableClientState(GL_VERTEX_ARRAY);
    // glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
}

void SpriteManager::StartDraw()
{
    Engine::renderer->NewFrame();
    Engine::renderer->Clear();
}

void SpriteManager::EndDraw()
{
    Engine::renderer->EndFrame();
}

Camera *camera = nullptr;
GameObject *cameraGo = nullptr;
MeshData *spriteMeshData = nullptr;

void SpriteManager::Init()
{
    spriteMeshData = new MeshData(4, 6);
    spriteMeshData->AddVertice(0.0f, 0.0f, 0xFFFFFFFF, -0.5f, -0.5f, 0.0f, 0);
    spriteMeshData->AddVertice(1.0f, 0.0f, 0xFFFFFFFF, 0.5f, -0.5f, 0.0f, 1);
    spriteMeshData->AddVertice(1.0f, 1.0f, 0xFFFFFFFF, 0.5f, 0.5f, 0.0f, 2);
    spriteMeshData->AddVertice(0.0f, 1.0f, 0xFFFFFFFF, -0.5f, 0.5f, 0.0f, 3);
    spriteMeshData->indices[0] = 0;
    spriteMeshData->indices[1] = 2;
    spriteMeshData->indices[2] = 1;
    spriteMeshData->indices[3] = 2;
    spriteMeshData->indices[4] = 0;
    spriteMeshData->indices[5] = 3;
#ifdef __PSP__
    sceKernelDcacheWritebackInvalidateAll(); // Very important
#endif
    cameraGo = new GameObject("Camera");
    camera = cameraGo->AddComponent<Camera>();
    // camera->SetProjectionType(Orthographic);
}

void SpriteManager::DrawSprite(Vector3 position, Vector3 rotation, Vector3 scale, Texture *texture)
{
    // texture = UiManager::fonts[0]->Characters[65].texture;
    // float scaleCoef = 100.0f / texture->GetPixelPerUnit() / 100.0f;
    float scaleCoef = (100.0f / 100.0f) / 100.0f;
    float w = texture->width * scaleCoef;
    float h = texture->height * scaleCoef;

    // Set settings
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);

    Engine::renderer->SetCameraPosition(camera);

    Vector3 pos = Vector3(-position.x, position.y, position.z);
    Vector3 rot = Vector3(rotation.x, rotation.y + 180, rotation.z);
    Vector3 scl = Vector3(scale.x * w, scale.y * h, 1);
    Engine::renderer->SetTransform(pos, rot, scl);

    texture->Bind();
    DrawMeshData(spriteMeshData);
}

std::vector<Font *> UiManager::fonts;

int UiManager::Init()
{
    return 0;
}

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
Font *UiManager::CreateFont(std::string filePath)
{
    //return nullptr;
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
            Texture *texture = new Texture();

#ifdef __PSP__
            int pW = pow22(face->glyph->bitmap.width);
            int pH = pow22(face->glyph->bitmap.rows);

            unsigned char *test = (unsigned char *)malloc(sizeof(unsigned char) * pW * pH * 4);
            if(face->glyph->bitmap.width != 0 && face->glyph->bitmap.rows != 0)
            {
                // Debug::Print("a " +  std::to_string(c) + " " + std::to_string(face->glyph->bitmap.width) + ", " + std::to_string(face->glyph->bitmap.rows));
                unsigned char *texData2 = (unsigned char *)malloc(pW * pH * 1);
                stbir_resize_uint8(face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, texData2, pW, pH, 0, 1);
                int size = pW * pH;
                for (int i = 0; i < size; i++)
                {
                    int off = size - i - 1;
                    test[0+i*4] = texData2[off];
                    test[1+i*4] = texData2[off];
                    test[2+i*4] = texData2[off];
                    test[3+i*4] = texData2[off];
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
                (unsigned int)face->glyph->advance.x};
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
