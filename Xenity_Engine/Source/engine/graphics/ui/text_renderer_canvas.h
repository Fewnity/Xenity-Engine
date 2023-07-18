#if defined(EXPORT)
#define API __declspec(dllexport)
#elif defined(IMPORT)
#define API __declspec(dllimport)
#else
#define API
#endif

#pragma once
#include <string>

#include "../../component.h"
#include "../iDrawable.h"
#include "TextAlignments.h"
#include "../color/color.h"
#include "../../vectors/vector3.h"
#include "../../engine.h"

class Font;
class TextInfo;
class MeshData;
// class Shader;

class API TextRendererCanvas : public IDrawable
{
public:
	TextRendererCanvas();
	~TextRendererCanvas();
	//void SetReflection();

	std::unordered_map<std::string, Variable> GetReflection();

	Vector3 position = Vector3(0, 0, 0);
	float size = 16;
	float lineSpacing = 0;
	float characterSpacing = 0;
	int GetDrawPriority() const;

	// Shader *shader = nullptr;
	HorizontalAlignment horizontalAlignment = H_Center;
	VerticalAlignment verticalAlignment = V_Center;

	void SetColor(Color color)
	{
		this->color = color;
	}

	void SetOrderInLayer(int orderInLayer)
	{
		this->orderInLayer = orderInLayer;
		Engine::drawOrderListDirty = true;
	}

	int GetOrderInLayer() const
	{
		return orderInLayer;
	}

	void SetText(std::string text);
	void SetFont(Font *font);

private:
	std::string text;
	Color color = Color();
	void Draw();
	int orderInLayer = 0;
	TextInfo *textInfo = nullptr;
	Font *font = nullptr;
	bool isTextInfoDirty = true;
	MeshData *mesh = nullptr;
};
