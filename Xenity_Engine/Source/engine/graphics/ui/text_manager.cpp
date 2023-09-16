#include "text_manager.h"
#include "../../../xenity.h"
#include "../3d_graphics/mesh_data.h"

#include "../renderer/renderer.h"
#include "font.h"

ProfilerBenchmark *textBenchmark = nullptr;

/**
 * @brief Init text manager
 *
 */
void TextManager::Init()
{
    textBenchmark = new ProfilerBenchmark("Text", "Text");
}

void TextManager::DrawTextMesh(std::shared_ptr<MeshData> mesh, bool for3D, bool invertFaces, std::shared_ptr <Texture> texture)
{
    // Set draw settings
    RenderingSettings renderSettings = RenderingSettings();
    renderSettings.invertFaces = invertFaces;
    renderSettings.useBlend = true;
    renderSettings.useDepth = for3D;
    renderSettings.useTexture = true;
    renderSettings.useLighting = for3D;

    std::vector< std::shared_ptr<Texture>> textures;
    textures.push_back(texture);
    Engine::renderer->DrawMeshData(mesh, textures, renderSettings);
}

void TextManager::SetTextPosition(std::weak_ptr<Transform> weakTransform, bool canvas)
{
    auto transform = weakTransform.lock();
    // Set text scale and pivot position/rotation
    Vector3 pos;
    if (!canvas)
    {
        pos = transform->GetPosition();
    }
    else
    {
        float xOff = (-Graphics::usedCamera.lock()->GetAspectRatio() * 5) + (transform->GetPosition().x * (Graphics::usedCamera.lock()->GetAspectRatio() * 10));
        float yOff = (-1 * 5) + (transform->GetPosition().y * (1 * 10));
        pos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane
    }

    Vector3 scl = transform->GetScale();
    scl.x = -scl.x;
    Vector3 rot = transform->GetRotation();
    Engine::renderer->SetTransform(pos, rot, scl, true);
}

std::shared_ptr <MeshData> TextManager::CreateMesh(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Color color, std::shared_ptr<Font> font)
{
    if (!font)
        return nullptr;

    textBenchmark->Start();
    int textLenght = (int)text.size();

    // if (textLenght == 0)
    // {
    //     textBenchmark->Stop();
    //     return;
    // }

    // Set text start offset
    float totalY = 0;
    for (int i = 0; i < textInfo->lineCount; i++)
    {
        totalY += textInfo->maxLineHeight;
    }

    float x = 0;
    float y = 0;
    int line = 0;
    if (horizontalAlignment == H_Left)
        x = -textInfo->linesInfo[line].lenght;
    else if (horizontalAlignment == H_Center)
        x = -textInfo->linesInfo[line].lenght * 0.5f;

    y = textInfo->linesInfo[line].y1 * 0.25f;
    y += -textInfo->maxLineHeight;

    if (verticalAlignment == V_Center)
    {
        y += totalY * 0.5f;
    }
    else if (verticalAlignment == V_Top)
    {
        y += totalY;
    }

    // Create empty mesh
    int charCountToDraw = textLenght - (textInfo->lineCount - 1);
    std::shared_ptr <MeshData> mesh = MeshData::MakeMeshData(4 * charCountToDraw, 6 * charCountToDraw, false, false, true);

    mesh->unifiedColor = color;

    int drawnCharIndex = 0;
    for (int i = 0; i < textLenght; i++)
    {
        char c = text[i];
        Character *ch = font->Characters[c];

        if (c == '\n')
        {
            line++;

            if (horizontalAlignment == H_Left)
                x = -textInfo->linesInfo[line].lenght;
            else if (horizontalAlignment == H_Center)
                x = -textInfo->linesInfo[line].lenght * 0.5f;
            else
                x = 0;

            y += -textInfo->maxLineHeight;
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
    textBenchmark->Stop();
    return mesh;
}

void TextManager::DrawText(std::string &text, TextInfo *textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, std::weak_ptr<Transform> weakTransform, Color color, bool canvas, std::shared_ptr <MeshData> mesh, std::shared_ptr<Font> font)
{
    if (!font)
        return;

    if (!font->fontAtlas || !font->fontAtlas->IsValid())
    {
        Debug::PrintError("[TextManager::DrawText] Invalid font");
        return;
    }

    if (auto cameraLock = Graphics::usedCamera.lock())
    {
        textBenchmark->Start();

        // Set projection
        if (!canvas)
        {
            Engine::renderer->SetCameraPosition(Graphics::usedCamera);
            Graphics::needUpdateCamera = false;
        }
        else
        {
            Engine::renderer->ResetView();
            Graphics::needUpdateCamera = true;
        }

        auto transform = weakTransform.lock();
        SetTextPosition(transform, canvas);

        bool invertFaces = false;
        if (transform->GetScale().x * transform->GetScale().y < 0)
            invertFaces = true;

        DrawTextMesh(mesh, !canvas, invertFaces, font->fontAtlas);
        textBenchmark->Stop();
    }
}

void TextManager::AddCharToMesh(std::shared_ptr <MeshData > mesh, Character *ch, float x, float y, int letterIndex)
{
    int indice = letterIndex * 4;
    int indiceIndex = letterIndex * 6;

    float w = ch->rightSize.x;
    float h = ch->rightSize.y;

    float fixedY = y - (ch->rightSize.y - ch->rightBearing.y);

    mesh->AddVertex(ch->uv.x, ch->uv.y, w + x, fixedY, 0, indice,0);
    mesh->AddVertex(ch->uvOffet.x, ch->uv.y, x, fixedY, 0, 1 + indice,0);
    mesh->AddVertex(ch->uvOffet.x, ch->uvOffet.y, x, h + fixedY, 0, 2 + indice,0);
    mesh->AddVertex(ch->uv.x, ch->uvOffet.y, w + x, h + fixedY, 0, 3 + indice,0);

    MeshData::SubMesh* subMesh = mesh->subMeshes[0];
    subMesh->indices[0 + indiceIndex] = 0 + indice;
    subMesh->indices[1 + indiceIndex] = 2 + indice;
    subMesh->indices[2 + indiceIndex] = 1 + indice;
    subMesh->indices[3 + indiceIndex] = 2 + indice;
    subMesh->indices[4 + indiceIndex] = 0 + indice;
    subMesh->indices[5 + indiceIndex] = 3 + indice;
}

TextInfo *TextManager::GetTextInfomations(std::string &text, int textLen, std::shared_ptr<Font> font, float scale)
{
    TextInfo *textInfos = new TextInfo();
    if (!font)
        return textInfos;

    textInfos->linesInfo.push_back(LineInfo());

    int currentLine = 0;
    float higherY = 0;
    float lowerY = 0;

    for (int i = 0; i < textLen; i++)
    {
        Character *ch = font->Characters[text[i]];
        if (text[i] == '\n')
        {
            textInfos->linesInfo[currentLine].lenght *= scale;
            textInfos->linesInfo[currentLine].y1 = (higherY - lowerY) * scale;
            textInfos->linesInfo.push_back(LineInfo());
            currentLine++;
            higherY = 0;
            lowerY = 0;
        }
        else
        {
            textInfos->linesInfo[currentLine].lenght += ch->rightAdvance;
            if (higherY < ch->rightBearing.y)
                higherY = ch->rightBearing.y;

            float low = ch->rightSize.y - ch->rightBearing.y;
            if (lowerY < low)
                lowerY = low;
        }
    }
    textInfos->linesInfo[currentLine].lenght *= scale;
    textInfos->linesInfo[currentLine].y1 = (higherY - lowerY) * scale;

    textInfos->maxLineHeight = font->maxCharHeight * scale;
    textInfos->lineCount = currentLine + 1;

    return textInfos;
}
