#pragma once

#include <memory>

class Vector3;
class Vector2;
class Color;
class Texture;

class Gizmo
{
public:
	static void Init();
	static void DrawLine(const Vector3& a, const Vector3& b);
	static void SetColor(const Color& newColor);
	static void DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color);
private:
	static Color color;
};

